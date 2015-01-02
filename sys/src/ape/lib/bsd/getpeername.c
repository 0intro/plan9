/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#include "priv.h"

static int
connected(Rock *r, int len)
{
	static struct sockaddr niladdr;

	return memcmp(&r->raddr, &niladdr, len) != 0;
}

int
getpeername(int fd, void *addr, int *alen)
{
	Rock *r;
	int i;
	struct sockaddr_un *runix;

	r = _sock_findrock(fd, 0);
	if(r == nil){
		errno = ENOTSOCK;
		return -1;
	}

	switch(r->domain){
	case PF_INET:
		*alen = sizeof(struct sockaddr_in);
		memmove(addr, &r->raddr, *alen);
		break;
	case PF_INET6:
		*alen = sizeof(struct sockaddr_in6);
		memmove(addr, &r->raddr, *alen);
		break;
	case PF_UNIX:
		runix = (struct sockaddr_un*)&r->raddr;
		i = &runix->sun_path[strlen(runix->sun_path)] - (char*)runix;
		memmove(addr, runix, i);
		*alen = i;
		break;
	default:
		errno = EAFNOSUPPORT;
		return -1;
	}

	if(!connected(r, *alen)){
		errno = ENOTCONN;
		memset(&addr, 0, *alen); /* python depends on this */
		return -1;
	}
	return 0;
}
