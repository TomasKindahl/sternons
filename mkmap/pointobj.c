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

#define _SZ_TYPE sizeof(int)
#define _SZ_RA sizeof(double)
#define _SZ_DE sizeof(double)
#define _SZ_V sizeof(double)
#define _SZ_HIP sizeof(int)
#define _SZ_prev sizeof(struct _pointobj_T *)

#define _A_TYPE 0
#define _A_RA (_A_TYPE + _SZ_TYPE)
#define _A_DE (_A_RA + _SZ_RA)
#define _A_V (_A_DE + _SZ_DE)
#define _A_HIP (_A_V + _SZ_V)
#define _A_prev (_A_HIP + _SZ_HIP)

pointobj *new_star(int HIP, double RA, double DE, double V, pointobj *prev) {
	/*star *res = ALLOC(star);*/
	/*star *res = (star *)malloc(_SZ_TYPE + _SZ_RA + _SZ_DE + _SZ_V
								 + _SZ_HIP + _SZ_prev);*/
	char *res = (char *)malloc(_SZ_TYPE + _SZ_RA + _SZ_DE + _SZ_V
							   + _SZ_HIP + _SZ_prev);
	int *IPTR; double *DPTR; struct _pointobj_T **PPTR;
	/*res->type = PO_STAR;*/
	IPTR = (int *)&res[_A_TYPE]; *IPTR = PO_STAR;
    /*res->RA = RA; res->DE = DE;*/
    DPTR = (double *)&res[_A_RA]; *DPTR = RA; 
    DPTR = (double *)&res[_A_DE]; *DPTR = DE; 
    /*res->V = V;*/
    DPTR = (double *)&res[_A_V]; *DPTR = V; 
    /*res->HIP = HIP;*/
	IPTR = (int *)&res[_A_HIP]; *IPTR = HIP;
    /*res->prev = prev;*/
    PPTR = (struct _pointobj_T **)&res[_A_prev]; *PPTR = prev;
    return (pointobj *)res;
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

#define QSORT_CMP(FIELD) \
	int pointobj_cmp_by_ ## FIELD(const void *P1, const void *P2) {	\
	    pointobj *S1 = *((pointobj **)P1);								\
	    pointobj *S2 = *((pointobj **)P2);								\
	    return (S1->FIELD > S2->FIELD) - (S1->FIELD < S2->FIELD);		\
	}

QSORT_CMP(V)
QSORT_CMP(HIP)

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

double attr_F(pointobj *PO, int attr) {
	switch(attr) {
		case POA_RA: return PO->RA;
		case POA_DE: return PO->DE;
		case POA_V: return PO->V;
		case POA_HIP:
		default:
			return -1;
	}
	return -1;
}

int attr_I(pointobj *PO, int attr) {
	star *S;
	switch(attr) {
		case POA_HIP:
			if (star_type(PO->type)) {
				S = (star *)PO;
				return S->HIP;
			}
			S = (star *)PO;
			return S->HIP;
		default:
			return -1;
	}
	return -1;
}

