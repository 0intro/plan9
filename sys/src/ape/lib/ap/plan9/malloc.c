#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <lock.h>

typedef unsigned int	uint;

enum
{
	MAGIC		= 0xbada110c,
	MAX2SIZE	= 32,
	CUTOFF		= 12,
};

typedef struct Bucket Bucket;
struct Bucket
{
	int	size;
	int	magic;
	Bucket	*next;
	int	pad;
	char	data[1];
};

typedef struct Arena Arena;
struct Arena
{
	Bucket	*btab[MAX2SIZE];	
	Lock;
};
static Arena arena;

#define datoff		((intptr_t)((Bucket*)0)->data)
#define nil		((void*)0)

extern	void	*sbrk(unsigned long);

void*
malloc(size_t size)
{
	uint next;
	int pow, n;
	Bucket *bp, *nbp;

	for(pow = 1; pow < MAX2SIZE; pow++) {
		if(size <= (1<<pow))
			goto good;
	}

	return nil;
good:
	/* Allocate off this list */
	lock(&arena);
	bp = arena.btab[pow];
	if(bp) {
		arena.btab[pow] = bp->next;
		unlock(&arena);

		if(bp->magic != 0)
			abort();

		bp->magic = MAGIC;
		return bp->data;
	}

	size = sizeof(Bucket)+(1<<pow);
	size += 7;
	size &= ~7;

	if(pow < CUTOFF) {
		n = (CUTOFF-pow)+2;
		bp = sbrk(size*n);
		if((intptr_t)bp < 0){
			unlock(&arena);
			return nil;
		}

		next = (intptr_t)bp+size;
		nbp = (Bucket*)next;
		arena.btab[pow] = nbp;
		for(n -= 2; n; n--) {
			next = (intptr_t)nbp+size;
			nbp->next = (Bucket*)next;
			nbp->size = pow;
			nbp = nbp->next;
		}
		nbp->size = pow;
	}
	else {
		bp = sbrk(size);
		if((intptr_t)bp == -1){
			unlock(&arena);
			return nil;
		}
	}
	unlock(&arena);
		
	bp->size = pow;
	bp->magic = MAGIC;

	return bp->data;
}

void
free(void *ptr)
{
	Bucket *bp, **l;

	if(ptr == nil)
		return;

	/* Find the start of the structure */
	bp = (Bucket*)((intptr_t)ptr - datoff);

	if(bp->magic != MAGIC)
		abort();

	bp->magic = 0;
	l = &arena.btab[bp->size];
	lock(&arena);
	bp->next = *l;
	*l = bp;
	unlock(&arena);
}

void*
realloc(void *ptr, size_t n)
{
	void *new;
	uint osize;
	Bucket *bp;

	if(ptr == nil)
		return malloc(n);

	/* Find the start of the structure */
	bp = (Bucket*)((intptr_t)ptr - datoff);

	if(bp->magic != MAGIC)
		abort();

	/* enough space in this bucket */
	osize = 1<<bp->size;
	if(osize >= n)
		return ptr;

	new = malloc(n);
	if(new == nil)
		return nil;

	memmove(new, ptr, osize);
	free(ptr);

	return new;
}
