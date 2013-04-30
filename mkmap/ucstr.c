/************************************************************************
 *  Copyright (C) 2010-2013 Tomas Kindahl                               *
 ************************************************************************
 *  This file is part of mkmap                                          *
 *                                                                      *
 *  mkmap is free software: you can redistribute it and/or modify it    *
 *  under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or   *
 *  (at your option) any later version.                                 *
 *                                                                      *
 *  mkmap is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *  GNU General Public License for more details.                        *
 *                                                                      *
 *  You should have received a copy of the GNU General Public License   *
 *  along with mkmap. If not, see <http://www.gnu.org/licenses/>.       *
 ************************************************************************/

#include <stdlib.h>
#include "meta.h"
#include "defs.h"
#include "uctype.h"

int ucslen(uchar *ustr) {
    int len = 0;
    while (ustr[len]) len++;
    return len;
}

uchar *ucscpy(uchar *dest, uchar *src) {
    int ix;
    for (ix = 0; src[ix]; ix++)
        dest[ix] = src[ix];
    dest[ix] = L'\0';
    return dest;
}

uchar *ucsncpy(uchar *dest, uchar *src, int num) {
    int ix, jx;

	/* Weird implementation acc2 standard libc */
    for (ix = jx = 0; ix < num; ix++) {
        dest[ix] = src[jx];
		if(src[jx]) jx++;
	}
    return dest;
}

uchar *ucsntcpy(uchar *dest, uchar *src, int max) {
    int ix;
	
	/* Null terminating AT MOST /max/ uchars copied */
    for (ix = 0; ix < max && src[ix]; ix++)
        dest[ix] = src[ix];
    dest[ix] = L'\0';
    return dest;
}

int ucscmp(uchar *ucs1, uchar *ucs2) {
    int ix;
    for (ix = 0; ucs1[ix] && ucs2[ix] && ucs1[ix] == ucs2[ix]; ix++);
    if (!ucs1[ix] && !ucs2[ix]) return 0;
    if (!ucs1[ix]) return -1;
    if (!ucs2[ix]) return 1;
    if (ucs1[ix] < ucs2[ix]) return -1;
    if (ucs1[ix] > ucs2[ix]) return 1;
    return 0;
}

uchar *ucschr(uchar *haystack, uchar needle) {
    while (*haystack && *haystack != needle) {
        haystack++;
    }
    return haystack;
}

uchar *ucsdup(uchar *src) {
    int len;
    uchar *res;

    if(!src) return 0;
    len = ucslen(src);
    res = ALLOCN(uchar, len+1);
    ucscpy(res, src);
    return res;
}

uchar *ucsndup(uchar *src, int max) {
    int len;
    uchar *res;

    if(!src) return 0;
    len = ucslen(src);
	if (len > max) len = max;
	res = ALLOCN(uchar, len+1);
    ucsntcpy(res, src, max);
    return res;
}

long _ucint(uchar uc) {
    return uc-L'0';
}

long ucstoi(uchar *ucs) {
    /* untested! */
    int sign = 1;
    long intpart = 0;

    switch(*ucs) {
        case L'-': sign = -1; ucs++; break;
        case L'+': sign = +1; ucs++; break;
        default: break;
    }
    while (isunum(*ucs)) {
        intpart = intpart*10 + _ucint(*ucs);
        ucs++;
    }
    return sign*(long)intpart;
}

double ucstof(uchar *ucs) {
    /* untested! */
    int sign = 1;
    long intpart = 0;
    double fracpart = 0; double decimal = 1;

    switch(*ucs) {
        case L'-': sign = -1; ucs++; break;
        case L'+': sign = +1; ucs++; break;
        default: break;
    }
    while (isunum(*ucs)) {
        intpart = intpart*10 + _ucint(*ucs);
        ucs++;
    }
    if (!*ucs) return sign*(double)intpart;
    if (*ucs == L'.') {
        ucs++;
        while (isunum(*ucs)) {
            fracpart = fracpart*10.0 + _ucint(*ucs);
            decimal = decimal*10.0;
            ucs++;
        }
    }
    /* Insert exponent detection here! */
    return sign*((double)intpart + fracpart/decimal);
}

