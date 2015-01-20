#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include "lib.h"
#include "sys9.h"

ssize_t
pwrite(int fd, const void *buf, size_t nbytes, off_t off)
{
	int n;

	if(fd<0 || fd>=OPEN_MAX || !(_fdinfo[fd].flags&FD_ISOPEN)){
		errno = EBADF;
		return -1;
	}
	if(_fdinfo[fd].oflags&O_APPEND)
		_SEEK(fd, 0, 2);
	n = _PWRITE(fd, buf, nbytes, off);
	if(n < 0)
		_syserrno();
	return n;
}

ssize_t
write(int fd, const void *buf, size_t nbytes)
{
	return pwrite(fd, buf, nbytes, -1ll);
}
