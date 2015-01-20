/*
 * pANS stdio -- fclose
 */
#include "iolib.h"
int fclose(FILE *f){
	int d, error=0;
	char *p;

	if(!f) return EOF;
	if(f->state==CLOSED) return EOF;
	if(fflush(f)==EOF) error=EOF;
	if(f->flags&BALLOC){
		if((p = f->buf)!=0){
			f->buf = 0;
			f->wp = 0;
			f->rp = 0;
			f->lp = 0;
			free(p);
		}
	}
	if(!(f->flags&STRING)){
		if((d = f->fd)>=0){
			f->fd = -1;
			if(close(d) < 0)
				error = EOF;
		}
	}
	f->state=CLOSED;
	f->flags=0;
	return error;
}
