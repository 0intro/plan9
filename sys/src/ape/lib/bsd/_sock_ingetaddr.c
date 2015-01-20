/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#include "priv.h"

void
_sock_ingetaddr(Rock *r, struct sockaddr *sa, int *alen, char *a)
{
	int n, fd;
	char *p;
	char name[Ctlsize];
	struct sockaddr_in *ip;
	struct sockaddr_in6 *ip6;

	/* get remote address */
	strcpy(name, r->ctl);
	p = strrchr(name, '/');
	strcpy(p+1, a);
	fd = open(name, O_RDONLY);
	if(fd < 0)
		return;
	n = read(fd, name, sizeof(name)-1);
	if(n < 0){
		close(fd);
		return;
	}
	name[n] = 0;
	if((p = strchr(name, '!')) != nil){
		*p++ = 0;

		/*
		 * allow a AF_INET listen to accept a AF_INET6 call
		 * so a machine with ip4 and ip6 addresses can accept either.
		 */
		if(strchr(p, ':') != nil){
			ip = (struct sockaddr_in*)sa;
			ip->sin_family = AF_INET;
			ip->sin_port = htons(atoi(p));
			ip->sin_addr.s_addr = inet_addr(name);
			if(alen)
				*alen = sizeof(struct sockaddr_in);
		}else{
			ip6 = (struct sockaddr_in6*)sa;
			ip6->sin6_family = AF_INET6;
			ip6->sin6_port = htons(atoi(p));
			inet_pton(AF_INET6, name, &ip6->sin6_addr);
			if(alen)
				*alen = sizeof(struct sockaddr_in6);
		}
	}
	close(fd);
}
