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


