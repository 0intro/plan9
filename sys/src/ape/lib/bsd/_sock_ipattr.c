/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "priv.h"

/*
 *  return ndb attribute type of an ip name
 */
int
_sock_ipattr(char *name)
{
	struct in6_addr ia6;

	if(inet_pton(AF_INET6, name, &ia6) == 1)
		return Tip;
	if(strchr(name, '.') != nil)
		return Tdom;
	return Tsys;
}
