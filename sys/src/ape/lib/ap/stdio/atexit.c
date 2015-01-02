#include <unistd.h>
#include <stdlib.h>

extern void (*_atexitfns[ATEXIT_MAX])(void);

int
atexit(void (*f)(void))
{
	int i;
	for(i=0; i<ATEXIT_MAX; i++)
		if(!_atexitfns[i]){
			_atexitfns[i] = f;
			return(0);
		}
	return(1);
}
