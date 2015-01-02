/*
 * pANS stdio -- sopenw
 */
#include "iolib.h"

FILE *_IO_sopenw(void){
	FILE *f;

	if((f=_IO_newfile())==NULL)
		return NULL;
	f->buf=f->rp=f->wp=0;
	f->state=OPEN;
	f->flags=STRING;
	f->fd=-1;
	return f;
}
