#include <stdlib.h>
#include <stdio.h>
#include "defs.h"
#include "usio.h"

utf8_file *u8fopen(char *fname) {
	utf8_file *res = (utf8_file *)malloc(sizeof(utf8_file));
	res->chfile = fopen(fname, "rt");
	if (!res->chfile) { free(res); return 0; }
	res->uchar_save = -1;
	return res;
}

int u8fclose(utf8_file *file) {
	FILE *inf = file->chfile;
	free(file);
	return fclose(inf);
}

int u8feof(utf8_file *file) {
	return feof(file->chfile);
}

uchar _fgetuc(FILE *stream) {
    uchar res = fgetc(stream);
    uchar n = res;
    int ix;
    int L[] = {0xE0, 0xF0, 0xF8, 0xFC, 0x100};
    uchar mask[] = {0x7FF, 0xFFFF, 0x1FFFFF, 0x3FFFFFF, 0x7FFFFFFF};

    if (res<0x80) return res;
    if (res<0xC0) return REPLACEMENT_CHAR;	/* DATA ERROR! */

    for (ix = 0; ix < 5; ix++) {
        res <<= 6;
		if (feof (stream)) return REPLACEMENT_CHAR; /* premature eof (ERROR) */
        res |= fgetc (stream)&0x3F;
        if (n<L[ix]) return mask[ix]&res;
    }
    return mask[ix] & res;
}

uchar fgetuc(utf8_file *stream) {
	int res;

	if (stream->uchar_save == -1) {
		return _fgetuc(stream->chfile);
	}
	res = stream->uchar_save;
	stream->uchar_save = -1;
	return res;
}

uchar fungetuc(int uch, utf8_file *stream) {
	stream->uchar_save = uch;
	return uch;
}

uchar *fgetus(uchar *us, int size, utf8_file *stream) {
	int ch;
	if (u8feof(stream)) return 0;
	ch = fgetuc(stream);
	if (u8feof(stream)) return 0;
	while (!u8feof(stream) && ch != L'\n') {
		*us++ = ch;
		ch = fgetuc(stream);
	}
	*us++ = ch;
	*us = L'\0';
	return us;
}

char *uctombs(char *dest, uchar src) {
    int lim[]  = {0x800, 0x10000, 0x200000, 0x4000000, 0x80000000};
    int S[]    = {0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
    int mask[] = {0x1F, 0x0F, 0x07, 0x03, 0x01};
    int ix, jx, shf;
      
    if (src < 0x80) {
        dest[0] = src; dest[1] = '\0';
        return dest;
    }
    for (ix = 0; ix < 5; ix += 1) {
        if (src < lim[ix]) {
            shf = (ix+1)*6;
            dest[0] = S[ix]|((src>>shf)&mask[ix]);
            for (jx = 1, shf-=6; jx < ix+2; jx += 1, shf-=6) {
                dest[jx] = 0x80|((src>>shf)&0x3F);
            }
            dest[jx] = 0x0;
            return dest;
        }
    }
    /* ERROR! => a replacement char is emitted. */
    fprintf (stderr, "WARNING: unrecognized uchar 0x%X\n", src);
    dest[0] = (char)0xEF; dest[1] = (char)0xBF;
    dest[2] = (char)0xBD; dest[3] = (char)0x0;
    return dest;
}

char *ucstombs(char *dest, uchar *src, int size) {
    uchar *s;
    char *d;
    for (s = src, d = dest; *s; s++) {
        uctombs(d, *s); /* enhances d up to 5 chars depending on s, null terminated */
        while (*d) d++; /* walk to that null terminus */
    }
    *d = '\0'; /* really? */
    return dest;
}


