/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>

/* socket extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

/* plan 9 */
#include "lib.h"
#include "sys9.h"

#include "priv.h"

int
bind(int fd, void *a, int alen)
{
	int n, len, cfd, port;
	Rock *r;
	char msg[128];

	/* assign the address */
	r = _sock_findrock(fd, 0);
	if(r == 0){
		errno = ENOTSOCK;
		return -1;
	}
	if(alen > sizeof(r->addr)){
		errno = ENAMETOOLONG;
		return -1;
	}
	memmove(&r->addr, a, alen);

	/* the rest is IP sepecific */
	switch(r->domain){
	case PF_INET:
	case PF_INET6:
		cfd = open(r->ctl, O_RDWR);
		if(cfd < 0){
			errno = EBADF;
			return -1;
		}
		_sock_ntop(r->domain, &r->addr, nil, 0, &port);
		if(port > 0)
			snprintf(msg, sizeof msg, "bind %d", port);
		else
			strcpy(msg, "bind *");
		n = write(cfd, msg, strlen(msg));
		if(n < 0){
			errno = EOPNOTSUPP;	/* Improve error reporting!!! */
			close(cfd);
			return -1;
		}
		close(cfd);

		if(port <= 0)
			_sock_ingetaddr(r, &r->addr, &len, "local");
	}

	return 0;
}
