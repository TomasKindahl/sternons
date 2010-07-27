#ifndef _USIO_H
#define _USIO_H

#include "defs.h"

#define REPLACEMENT_CHAR 0xFFFD

typedef struct _utf8_file_S {
	/* specialized token file allowing token get and unget */
	FILE *chfile;
	uchar uchar_save;
} utf8_file;

utf8_file *u8fopen(char *fname);		/* DO: "rt", "wt" */
int u8fclose(utf8_file *file);
int u8feof(utf8_file *file);
uchar fgetuc(utf8_file *stream);
uchar fungetuc(int uch, utf8_file *stream);
uchar *fgetus(uchar *us, int size, utf8_file *stream);

/* DO: fputuc, fputus */
char *uctombs(char *dest, uchar src);
char *ucstombs(char *dest, uchar *src, int size);

#endif /* _USIO_H */