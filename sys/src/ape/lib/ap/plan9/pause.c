#include "lib.h"
#include <unistd.h>
#include <errno.h>
#include "sys9.h"

int
pause(void)
{
	for(;;)
		if(_SLEEP(1000*1000) < 0){
			errno = EINTR;
			return -1;
		}
}
