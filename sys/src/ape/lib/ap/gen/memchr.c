#include	<string.h>

void*
memchr(const void *ap, int c, size_t n)
{
	const unsigned char *sp;

	sp = ap;
	c &= 0xFF;
	while(n > 0) {
		if(*sp++ == c)
			return (void*)(sp-1);
		n--;
	}
	return 0;
}
