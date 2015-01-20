#include <math.h>
#include <errno.h>
#define _RESEARCH_SOURCE
#include <float.h>

#define	MASK	0x7ffL
#define	SHIFT	20
#define	BIAS	1022L
#define	SIG	52

double
frexp(double d, int *ep)
{
	FPdbleword x, a;

	*ep = 0;
	if(isNaN(d) || isInf(d, 0) || d == 0)
		return d;
	x.x = d;
	a.x = fabs(d);
	if((a.hi >> SHIFT) == 0){
		/* normalize subnormal numbers */
		x.x = (double)(1ULL<<SIG) * d;
		*ep = -SIG;
	}
	*ep += ((x.hi >> SHIFT) & MASK) - BIAS;
	x.hi &= ~(MASK << SHIFT);
	x.hi |= BIAS << SHIFT;
	return x.x;
}

double
ldexp(double d, int e)
{
	FPdbleword x;

	if(d == 0)
		return 0;
	x.x = d;
	e += (x.hi >> SHIFT) & MASK;
	if(e <= 0)
		return 0;
	if(e >= MASK){
		errno = ERANGE;
		if(d < 0)
			return -HUGE_VAL;
		return HUGE_VAL;
	}
	x.hi &= ~(MASK << SHIFT);
	x.hi |= (long)e << SHIFT;
	return x.x;
}

double
modf(double d, double *ip)
{
	double f;
	FPdbleword x;
	int e;

	if(d < 1) {
		if(d < 0) {
			f = modf(-d, ip);
			*ip = -*ip;
			return -f;
		}
		*ip = 0;
		return d;
	}
	x.x = d;
	e = ((x.hi >> SHIFT) & MASK) - BIAS;
	if(e <= SHIFT+1) {
		x.hi &= ~(0x1fffffL >> e);
		x.lo = 0;
	} else
	if(e <= SHIFT+33)
		x.lo &= ~(0x7fffffffL >> (e-SHIFT-2));
	*ip = x.x;
	return d - x.x;
}
