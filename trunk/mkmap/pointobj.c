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

#define USE_VT_ARRS 1

typedef struct _star_T {
    int type;
    double RA, DE;
    double V;
    int HIP; /* 2mv */
    struct _pointobj_T *prev;
    /* 2Bdef: int cat; -- catalogue */
} star;

#define _SZ_TYPE sizeof(int)
#define _SZ_prev sizeof(struct _pointobj_T *)
#define _SZ_RA   sizeof(double)
#define _SZ_DE   sizeof(double)
#define _SZ_V    sizeof(double)
#define _SZ_HIP  sizeof(int)

#define _STAR_A_TYPE  0
#define _STAR_A_prev  ( _STAR_A_TYPE + _SZ_TYPE )
#define _STAR_A_RA    ( _STAR_A_prev + _SZ_prev )
#define _STAR_A_DE    ( _STAR_A_RA   + _SZ_RA   )
#define _STAR_A_V     ( _STAR_A_DE   + _SZ_DE   )
#define _STAR_A_HIP   ( _STAR_A_V    + _SZ_V    )

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

void init_types(void) {
    _sz[POA_none] = 0;
    _sz[POA_TYPE] = sizeof(int);
    _sz[POA_prev] = sizeof(void *);
    _sz[POA_RA] = sizeof(double);
    _sz[POA_RA_delta] = sizeof(double);
    _sz[POA_DE] = sizeof(double);
    _sz[POA_DE_delta] = sizeof(double);
    _sz[POA_PX] = sizeof(double);
    _sz[POA_RV] = sizeof(double);
    _sz[POA_V] = sizeof(double);
    _sz[POA_V_max] = sizeof(double);
    _sz[POA_V_min] = sizeof(double);
    _sz[POA_diam] = sizeof(double);
    _sz[POA_ell] = sizeof(double);
    _sz[POA_aspect] = sizeof(double);
    _sz[POA_SP] = sizeof(char[8]);
    _sz[POA_PN_morph] = sizeof(char[8]);
    _sz[POA_gxy_morph] = sizeof(char[8]);
    _sz[POA_neb_class] = sizeof(char[8]);
    _sz[POA_subobjs] = sizeof(int);
    _sz[POA_desg] = sizeof(char[8]);
    _sz[POA_HIP] = sizeof(int);
    _sz[POA_HD] = sizeof(int);
}

void init_star_class(void) {
    int ix;
    for (ix = 0; ix < POA; ix++) {
        _star_VT[ix] = -1;
    }
    _star_VT[POA_TYPE] = 0;
    _star_VT[POA_prev] = _star_VT[POA_TYPE] + _SZ_TYPE;
    _star_VT[POA_RA]   = _star_VT[POA_prev] + _SZ_prev;
    _star_VT[POA_DE]   = _star_VT[POA_RA]   + _SZ_RA;
    _star_VT[POA_V]    = _star_VT[POA_DE]   + _SZ_DE;
    _star_VT[POA_HIP]  = _star_VT[POA_V]    + _SZ_V;
    _star_VT[POA_size] = _star_VT[POA_HIP]  + _SZ_HIP;
    return;
}

pointobj *new_star(int HIP, double RA, double DE, double V, pointobj *prev) {
#if USE_VT_ARRS == 1
    byte *res = (byte *)malloc(_star_VT[POA_size]);
    _iset(res, _star_VT[POA_TYPE], PO_STAR);
    _pset(res, _star_VT[POA_prev], prev);
    _dset(res, _star_VT[POA_RA], RA);
    _dset(res, _star_VT[POA_DE], DE);
    _dset(res, _star_VT[POA_V], V);
    _iset(res, _star_VT[POA_HIP], HIP);
#else
    byte *res = (byte *)malloc(_SZ_TYPE + _SZ_RA + _SZ_DE + _SZ_V
                               + _SZ_HIP + _SZ_prev);
    _iset(res, _STAR_A_TYPE, PO_STAR);
    _pset(res, _STAR_A_prev, prev);
    _dset(res, _STAR_A_RA, RA);
    _dset(res, _STAR_A_DE, DE);
    _dset(res, _STAR_A_V, V);
    _iset(res, _STAR_A_HIP, HIP);
#endif
    return (pointobj *)res;
}

void init_classes(void) {
    init_types();
    init_star_class();
}

pointobj *new_pointobj(int type, int HIP, double RA, double DE, double V, pointobj *prev) {
    pointobj *res;
    switch (type) {
        case PO_STAR:
            return new_star(HIP, RA, DE, V, prev);
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
#if USE_VT_ARRS == 1
    double V1 = _dget(S1,_star_VT[POA_V]);
    double V2 = _dget(S2,_star_VT[POA_V]);
#else
    double V1 = _dget(S1,_STAR_A_V);
    double V2 = _dget(S2,_STAR_A_V);
#endif
    return (V1 > V2) - (V1 < V2);
}

int pointobj_cmp_by_HIP(const void *P1, const void *P2) {
    pointobj *S1 = *((pointobj **)P1);
    pointobj *S2 = *((pointobj **)P2);
#if USE_VT_ARRS == 1
    int H1 = _iget(S1,_star_VT[POA_HIP]);
    int H2 = _iget(S2,_star_VT[POA_HIP]);
#else
    int H1 = _iget(S1,_STAR_A_HIP);
    int H2 = _iget(S2,_STAR_A_HIP);
#endif
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
#if USE_VT_ARRS == 1
                return _dget(PO,_star_VT[POA_RA]);
#else
                return _dget(PO,_STAR_A_RA);
#endif
            }
           return PO->RA;
        case POA_DE: 
            if (PO->type == PO_STAR) {
#if USE_VT_ARRS == 1
                return _dget(PO,_star_VT[POA_DE]);
#else
                return _dget(PO,_STAR_A_DE);
#endif
            }
           return PO->DE;
        case POA_V:
            if (PO->type == PO_STAR) {
#if USE_VT_ARRS == 1
                return _dget(PO,_star_VT[POA_V]);
#else
                return _dget(PO,_STAR_A_V);
#endif
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
#if USE_VT_ARRS == 1
                return _iget(PO,_star_VT[POA_HIP]);
#else
                return _iget(PO,_STAR_A_HIP);
#endif
            }
            S = (star *)PO;
            return S->HIP;
        default:
            return -1;
    }
    return -1;
}

