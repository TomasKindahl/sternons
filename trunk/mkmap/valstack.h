/************************************************************************
 *  Copyright (C) 2010 Tomas Kindahl                                    *
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

#ifndef _VALSTACK_H
#define _VALSTACK_H

#define VS_INT 0
#define VS_DBL 1
#define VS_USTR 2
#define VS_CSTR 3

typedef struct _valstack_S {
    int type;
    union {
        int int_val;
        double dbl_val;
        uchar *ustr_val;
        char *cstr_val;
    } V;
    struct _valstack_S *prev;
} valstack;

#define VS_DEFS(TYPE,CTYPE) \
    valstack *VS_push_ ## TYPE(CTYPE int_val, valstack *prev); \
    int VS_is_ ## TYPE(valstack *vstack); \
    CTYPE VS_ ## TYPE(valstack *vstack);

VS_DEFS(int,int)
VS_DEFS(dbl,double)
VS_DEFS(ustr,uchar *)
VS_DEFS(cstr,char *)

valstack *VS_pop(valstack *vstack);
/* USAGE: always vstack = VS_pop(valstack)! */

#endif /* _VALSTACK_H */
