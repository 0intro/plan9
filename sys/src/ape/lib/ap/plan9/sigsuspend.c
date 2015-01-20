#include <signal.h>
#include <errno.h>

/*
 * BUG: doesn't work
 */

int
sigsuspend(sigset_t *set)
{
	USED(set);
	errno = EINVAL;
	return -1;
}
