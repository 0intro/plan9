#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

extern char **environ;

static int
envredo(char *s)
{
	char *x, **p, *q, **v, *mem;
	int n, nsz, nenv, esz;

	x = strchr(s, '=');
	if(x == NULL)
		return -1;
	nsz = x - s + 1;		/* compare the var=, not just var */

	nenv = 1;
	esz = strlen(s)+1;
	for(p = environ; (q = *p) != NULL; p++){
		esz += strlen(q)+1;
		nenv++;
	}

	/* overestimate in the case we have changed a variable. */
	v = malloc((nenv+1)*sizeof(char**) + esz);
	if(v == NULL)
		return -1;
	mem = (char*)(v + nenv + 1);

	nenv = 0;
	for(p = environ; (q = *p) != NULL; p++){
		if(strncmp(q, s, nsz) == 0)
			continue;
		v[nenv++] = mem;
		n = strlen(q)+1;
		memcpy(mem, q, n);
		mem += n;
	}
	v[nenv++] = mem;
	n = strlen(s)+1;
	memcpy(mem, s, n);

	v[nenv] = NULL;

	free(environ);
	environ = v;

	return 0;
}

int
putenv(char *s)
{
	int f, n;
	char *value;
	char buf[300];

	value = strchr(s, '=');
	if (value) {
		n = value-s;
		if(n<=0 || n > sizeof(buf)-6)
			return -1;
		strcpy(buf, "/env/");
		strncpy(buf+5, s, n);
		buf[n+5] = 0;
		f = creat(buf, 0666);
		if(f < 0)
			return 1;
		value++;
		n = strlen(value);
		if(write(f, value, n) != n)
			return -1;
		close(f);
		return envredo(s);
	} else
		return -1;
}
