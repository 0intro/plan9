/* posix */
#include <sys/types.h>
#include <unistd.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int h_errno;

struct hostent*
gethostbyaddr(void *addr, int len, int af)
{
	char name[64];

	USED(len);
	return gethostbyname(inet_ntop(af, addr, name, sizeof name));
}
