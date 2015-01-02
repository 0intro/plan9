#include <unistd.h>
#include <stdlib.h>

void (*_atexitfns[ATEXIT_MAX])(void);

void
_doatexits(void)
{
	int i;
	void (*f)(void);

	for(i = ATEXIT_MAX-1; i >= 0; i--)
		if(_atexitfns[i]){
			f = _atexitfns[i];
			_atexitfns[i] = NULL;	/* self defense against bozos */
			(*f)();
		}
}

void exit(int status)
{
	_doatexits();
	_exit(status);
}
