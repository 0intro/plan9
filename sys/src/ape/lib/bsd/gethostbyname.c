/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "priv.h"

int h_errno;

enum
{
	Nname= 6,
};

/*
 *  for inet addresses only
 */
struct hostent*
gethostbyname(char *name)
{
	int i, t, fd, m;
	char *p, *bp;
	int nn, na, na6;
	static struct hostent h;
	static char buf[1024];
	static char *nptr[Nname+1];
	static char *aptr[Nname+1];
	static char addr4[Nname][4];
	static char addr6[Nname][16];

	h.h_name = 0;
	t = _sock_ipattr(name);

	/* connect to server */
	fd = open("/net/cs", O_RDWR);
	if(fd < 0){
		_syserrno();
		h_errno = NO_RECOVERY;
		return 0;
	}

	/* construct the query, always expect an ip# back */
	switch(t){
	case Tsys:
		snprintf(buf, sizeof buf, "!sys=%s ip=*", name);
		break;
	case Tdom:
		snprintf(buf, sizeof buf, "!dom=%s ip=*", name);
		break;
	case Tip:
		snprintf(buf, sizeof buf, "!ip=%s", name);
		break;
	}

	/* query the server */
	if(write(fd, buf, strlen(buf)) < 0){
		_syserrno();
		h_errno = TRY_AGAIN;
		return 0;
	}
	lseek(fd, 0, 0);
	for(i = 0; i < sizeof(buf)-1; i += m){
		m = read(fd, buf+i, sizeof(buf) - 1 - i);
		if(m <= 0)
			break;
		buf[i+m++] = ' ';
	}
	close(fd);
	buf[i] = 0;

	/* parse the reply */
	nn = na = na6 = 0;
	for(bp = buf;;){
		p = strchr(bp, '=');
		if(p == 0)
			break;
		*p++ = 0;
		if(strcmp(bp, "dom") == 0){
			if(h.h_name == 0)
				h.h_name = p;
			if(nn < Nname)
				nptr[nn++] = p;
		} else if(strcmp(bp, "sys") == 0){
			if(nn < Nname)
				nptr[nn++] = p;
		} else if(strcmp(bp, "ip") == 0){
			if(strchr(p, ':') != nil){
				if(inet_pton(AF_INET6, p, addr6[na6]) == 1)
					na6++;
			}else{
				if(inet_pton(AF_INET, p, addr4[na]) == 1)
					na++;
			}
		}
		while(*p && *p != ' ')
			p++;
		if(*p)
			*p++ = 0;
		bp = p;
	}
	if(nn+na+na6 == 0){
		h_errno = HOST_NOT_FOUND;
		return 0;
	}

	nptr[nn] = 0;

	if(na == 0){
		for(i = 0; i < na6; i++)
			aptr[i] = addr6[i];
		aptr[i] = 0;
		h.h_addrtype = AF_INET6;
		h.h_length = 16;
	}else{
		for(i = 0; i < na; i++)
			aptr[i] = addr4[i];
		aptr[i] = 0;
		h.h_addrtype = AF_INET;
		h.h_length = 4;
	}

	h.h_aliases = nptr;
	h.h_addr_list = aptr;
	if(h.h_name == 0)
		h.h_name = nptr[0];
	if(h.h_name == 0)
		h.h_name = aptr[0];

	return &h;
}
