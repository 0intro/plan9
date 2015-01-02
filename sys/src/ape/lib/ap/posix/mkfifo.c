#include	<sys/types.h>
#include	<sys/stat.h>
#include	<errno.h>

int
mkfifo(char *path, mode_t mode)
{
	USED(path, mode);
	errno = 0;
	return -1;
}
