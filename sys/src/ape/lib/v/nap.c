extern int _SLEEP(long);

/*
 * from nap(2), 8th edition.  assume 60Hz
 * won't overflow.  n can be no bigger than 2*60.
 *
 * obsolete.
 */
int
nap(int n)
{
	_SLEEP((n*16667)/1000);
	return 0;
}
