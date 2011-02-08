#include <stdlib.h>
#include <stdio.h>
#include "star.h"

star *new_star(star *prev, int HIP, double RA, double DE, double vmag) {
    star *res = (star *)malloc(sizeof(star));
    res->RA = RA; res->DE = DE; res->vmag = vmag;
    res->prev = prev;
    res->HIP = HIP;
    return res;
}

star_view *new_star_view(int size) {
    star_view *res = (star_view *)malloc(sizeof(star_view));
    res->size = size;
    res->next = 0;
    res->S = (star **)malloc(sizeof(star *)*size);
    return res;
}

int append_star(star_view *SV, star *S) {
    if (SV->next == SV->size) {
        int newsize = SV->size<<1; /* double it */
        int ix;
        star **NS = (star **)malloc(sizeof(star *)*newsize);
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

int star_cmp_by_mag(const void *P1, const void *P2) {
	star *S1 = *((star **)P1);
	star *S2 = *((star **)P2);
	return (S1->vmag > S2->vmag) - (S1->vmag < S2->vmag);
}

int star_cmp_by_HIP(const void *P1, const void *P2) {
	star *S1 = *((star **)P1);
	star *S2 = *((star **)P2);
	return (S1->HIP > S2->HIP) - (S1->HIP < S2->HIP);
}

void dump_stars(FILE *stream, star *S) {
    fprintf(stream, "  (star: HIP =% 7i, α = %12.8f, δ = %12.8f, m = %4.2f)\n",
            S->HIP, S->RA, S->DE, S->vmag);
}

void dump_stars_view(FILE *stream, star_view *SV) {
    int ix;
    fprintf(stream, "stars_by_mag: size=%i, alloc=%i {\n", SV->next, SV->size);
    for(ix = 0; ix < SV->next; ix++) {
        dump_stars(stream, SV->S[ix]);
    }
    fprintf(stream, "}\n");
}

