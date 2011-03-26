#include <stdlib.h>
#include <stdio.h>
#include "meta.h"
#include "pointobj.h"

pointobj *new_pointobj(int type, int HIP, double RA, double DE, double vmag, pointobj *prev) {
	pointobj *res = ALLOC(pointobj);
    res->type = type;
    res->RA = RA; res->DE = DE; res->vmag = vmag;
    res->prev = prev;
    res->HIP = HIP;
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

QSORT_CMP(vmag)
QSORT_CMP(HIP)

void dump_pointobjs(FILE *stream, pointobj *S) {
    fprintf(stream, "  (star: HIP =% 7i, α = %12.8f, δ = %12.8f, m = %4.2f)\n",
            S->HIP, S->RA, S->DE, S->vmag);
}

void dump_pointobj_view(FILE *stream, pointobj_view *SV) {
    int ix;
    fprintf(stream, "stars_by_mag: size=%i, alloc=%i {\n", SV->next, SV->size);
    for(ix = 0; ix < SV->next; ix++) {
        dump_pointobjs(stream, SV->S[ix]);
    }
    fprintf(stream, "}\n");
}

