#include "lib.h"
#include <errno.h>
#include <inttypes.h>
#include "sys9.h"

char	end[];
static	char	*bloc = { end };

enum
{
	Round	= 7
};

char *
brk(char *p)
{
	uintptr_t bl;

	bl = ((uintptr_t)p + Round) & ~Round;
	if(_BRK_((void*)bl) < 0){
		errno = ENOMEM;
		return (char *)-1;
	}
	bloc = (char*)bl;
	return 0;
}

void *
sbrk(unsigned long n)
{
	uintptr_t bl;

	bl = ((uintptr_t)bloc + Round) & ~Round;
	if(_BRK_((void *)(bloc+n)) < 0){
		errno = ENOMEM;
		return (void *)-1;
	}
	bloc = (char*)bl + n;
	return (void*)bl;
}
