#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include "lib.h"
#include "sys9.h"

#include <stdio.h>

ssize_t
pread(int fd, void *buf, size_t nbytes, off_t offset)
{
	int n, noblock, isbuf;
	Fdinfo *f;

	if(fd<0 || fd>=OPEN_MAX || !((f = &_fdinfo[fd])->flags&FD_ISOPEN)){
		errno = EBADF;
		return -1;
	}
	if(nbytes <= 0)
		return 0;
	if(buf == 0){
		errno = EFAULT;
		return -1;
	}
	noblock = f->oflags&O_NONBLOCK;
	isbuf = f->flags&(FD_BUFFERED|FD_BUFFEREDX);
	if(noblock || isbuf){
		if(f->flags&FD_BUFFEREDX) {
			errno = EIO;
			return -1;
		}
		if(!isbuf) {
			if(_startbuf(fd) != 0) {
				errno = EIO;
				return -1;
			}
		}
		n = _readbuf(fd, buf, nbytes, noblock);
	}else{
		n = _PREAD(fd,  buf, nbytes, offset);
		if(n < 0)
			_syserrno();
	}
	return n;
}

ssize_t
read(int fd, void *buf, size_t nbytes)
{
	return pread(fd, buf, nbytes, -1ll);
}
