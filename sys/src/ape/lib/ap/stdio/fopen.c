/*
 * pANS stdio -- fopen
 */
#include "iolib.h"

FILE *fopen(const char *name, const char *mode){
	FILE *f;

	if((f = _IO_newfile()) == NULL)
		return NULL;
	return freopen(name, mode, f);
}
