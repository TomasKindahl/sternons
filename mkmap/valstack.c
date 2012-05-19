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

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "meta.h"
#include "valstack.h"

#define _VS_DEFS(TYPE,CTYPE,TYPENUM) \
    valstack *VS_push_ ## TYPE(CTYPE TYPE ## _val, valstack *prev) { \
        valstack *res = ALLOC(valstack); \
        res->type = TYPENUM; \
        res->V. TYPE ## _val = TYPE ## _val; \
        res->prev = prev; \
        return res; \
    } \
    int VS_is_ ## TYPE (valstack *vstack) { return vstack->type == TYPENUM; } \
    CTYPE VS_ ## TYPE (valstack *vstack) { \
        if(!VS_is_ ## TYPE(vstack)) { \
            fprintf(stderr, "FATAL: vstack bottom is not %s \t", # TYPE); \
            exit(-1); \
        } \
        return vstack->V. TYPE ## _val; \
    }

_VS_DEFS(int,int,VS_INT)
_VS_DEFS(dbl,double,VS_DBL)
_VS_DEFS(ustr,uchar *,VS_USTR)
_VS_DEFS(cstr,char *,VS_CSTR)

valstack *VS_pop(valstack *vstack) {
    return vstack->prev;
}


