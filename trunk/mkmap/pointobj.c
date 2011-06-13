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

#include <stdlib.h>
#include <stdio.h>
#include "meta.h"
#include "pointobj.h"

typedef struct _star_T {
    int type;
    double RA, DE;
    double V;
    int HIP; /* 2mv */
    struct _pointobj_T *prev;
    /* 2Bdef: int cat; -- catalogue */
} star;

#define byte char

/** Object layout independent set/get for int, double, voidptr **/
void _iset(byte *res, int index, int ival) {
    int *IPTR = (int *)&res[index];
    *IPTR = ival;
}

void _dset(byte *res, int index, double dval) {
    double *DPTR = (double *)&res[index];
    *DPTR = dval;
}

void _pset(byte *res, int index, void *vval) {
    void **VPTR = (void **)&res[index];
    *VPTR = vval;
}

int _iget(void *res, int index) {
    return *((int *)(&((byte *)res)[index]));
}

double _dget(void *res, int index) {
    return *((double *)(&((byte *)res)[index]));
}

void * _vget(void *res, int index) {
    return *((void **)(&((byte *)res)[index]));
}

int _sz[POA];
int _star_VT[POA];

#define _SZ_INT sizeof(int)
#define _SZ_PTR sizeof(void *)
#define _SZ_DBL sizeof(double)
#define _SZ_CH8 sizeof(char[8])

void init_method_tags(void) {
    _sz[POA_none] = 0;
    _sz[POA_TYPE] = _SZ_INT;
    _sz[POA_prev] = _SZ_PTR;
    _sz[POA_RA] = _SZ_DBL;
    _sz[POA_RA_delta] = _SZ_DBL;
    _sz[POA_DE] = _SZ_DBL;
    _sz[POA_DE_delta] = _SZ_DBL;
    _sz[POA_PX] = _SZ_DBL;
    _sz[POA_RV] = _SZ_DBL;
    _sz[POA_V] = _SZ_DBL;
    _sz[POA_V_max] = _SZ_DBL;
    _sz[POA_V_min] = _SZ_DBL;
    _sz[POA_diam] = _SZ_DBL;
    _sz[POA_ell] = _SZ_DBL;
    _sz[POA_aspect] = _SZ_DBL;
    _sz[POA_SP] = _SZ_CH8;
    _sz[POA_PN_morph] = _SZ_CH8;
    _sz[POA_gxy_morph] = _SZ_CH8;
    _sz[POA_neb_class] = _SZ_CH8;
    _sz[POA_subobjs] = _SZ_INT;
    _sz[POA_desg] = _SZ_CH8;
    _sz[POA_HIP] = _SZ_INT;
    _sz[POA_HD] = _SZ_INT;
}

void init_class(int tag[]) {
	int ix;
    for (ix = 0; ix < POA; ix++) {
        _star_VT[ix] = -1;
    }
    _star_VT[tag[0]] = 0;
    for (ix = 1; tag[ix] != POA_none; ix++) {
        _star_VT[tag[ix]] = _star_VT[tag[ix-1]] + _sz[tag[ix-1]];
    }
}

void init_star_class(void) {
    int tag[] = {
        POA_TYPE, POA_prev, POA_RA, POA_DE,
        POA_V, POA_HIP, POA_size, POA_none
    };
    init_class(tag);
    return;
}

pointobj *new_obj(int TYPE, double RA, double DE, double V, pointobj *prev) {
    byte *res = (byte *)malloc(_star_VT[POA_size]);
    _iset(res, _star_VT[POA_TYPE], TYPE);
    _pset(res, _star_VT[POA_prev], prev);
    _dset(res, _star_VT[POA_RA], RA);
    _dset(res, _star_VT[POA_DE], DE);
    _dset(res, _star_VT[POA_V], V);
    return (pointobj *)res;
}

void init_classes(void) {
    init_method_tags();
    init_star_class();
}

pointobj *new_pointobj(int type, int HIP, double RA, double DE, double V, pointobj *prev) {
    pointobj *res;
    switch (type) {
        case PO_STAR:
            res = new_obj(PO_STAR, RA, DE, V, prev);
            _iset((byte *)res, _star_VT[POA_HIP], HIP);
            return res;
        default:
            res = ALLOC(pointobj);
            res->type = PO_ANY;
            res->RA = RA; res->DE = DE;
            res->V = V;
            res->prev = prev;
            res->HIP = HIP;
    }
    return res;
}

pointobj_view *new_pointobj_view(int size) {
    pointobj_view *res = ALLOC(pointobj_view);
    res->size = size;
    res->next = 0;
    res->S = ALLOCN(pointobj *,size);
    return res;
}

int append_pointobj(VIEW(pointobj) *SV, pointobj *S) {
    if (SV->next == SV->size) {
        int newsize = SV->size<<1; /* double it */
        int ix;
        pointobj **NS = ALLOCN(pointobj *, newsize);
        for(ix = 0; ix < SV->size; ix++) {
            NS[ix] = SV->S[ix];
        }
        free(SV->S);
        SV->S = NS;
        SV->size = newsize;
        SV->S[SV->next++] = S;
        return 1;
    }
    SV->S[SV->next++] = S;
    return 0;
}

int pointobj_cmp_by_V(const void *P1, const void *P2) {
    pointobj *S1 = *((pointobj **)P1);
    pointobj *S2 = *((pointobj **)P2);
    double V1 = _dget(S1,_star_VT[POA_V]);
    double V2 = _dget(S2,_star_VT[POA_V]);
    return (V1 > V2) - (V1 < V2);
}

int pointobj_cmp_by_HIP(const void *P1, const void *P2) {
    pointobj *S1 = *((pointobj **)P1);
    pointobj *S2 = *((pointobj **)P2);
    int H1 = _iget(S1,_star_VT[POA_HIP]);
    int H2 = _iget(S2,_star_VT[POA_HIP]);
    return (H1 > H2) - (H1 < H2);
}

void dump_pointobjs(FILE *stream, pointobj *S) {
    fprintf(stream, "  (star: HIP =% 7i, α = %12.8f, δ = %12.8f, m = %4.2f)\n",
            S->HIP, S->RA, S->DE, S->V);
}

void dump_pointobj_view(FILE *stream, pointobj_view *SV) {
    int ix;
    fprintf(stream, "stars_by_mag: size=%i, alloc=%i {\n", SV->next, SV->size);
    for(ix = 0; ix < SV->next; ix++) {
        dump_pointobjs(stream, SV->S[ix]);
    }
    fprintf(stream, "}\n");
}

int star_type(int type) {
    int gtype;
    gtype = type & 0xF000;
    if (gtype == 0x1000) return 1;
    gtype = type & 0xFF00;
    if (gtype <= 0x2100) return 1;
    return 0;
}

double pointobj_attr_D(pointobj *PO, int attr) {
    switch(attr) {
        case POA_RA: 
            if (PO->type == PO_STAR) {
                return _dget(PO,_star_VT[POA_RA]);
            }
           return PO->RA;
        case POA_DE: 
            if (PO->type == PO_STAR) {
                return _dget(PO,_star_VT[POA_DE]);
            }
           return PO->DE;
        case POA_V:
            if (PO->type == PO_STAR) {
                return _dget(PO,_star_VT[POA_V]);
            }
           return PO->V;
        default:
            return -1;
    }
    return -1;
}

int pointobj_attr_I(pointobj *PO, int attr) {
    star *S;
    switch(attr) {
        case POA_HIP:
            if (PO->type == PO_STAR) {
                return _iget(PO,_star_VT[POA_HIP]);
            }
            S = (star *)PO;
            return S->HIP;
        default:
            return -1;
    }
    return -1;
}

