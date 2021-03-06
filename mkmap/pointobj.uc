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
#include <stdio.h>
#include "meta.h"
#include "defs.h"
#include "allstrings.h"
#include "ucstr.h"
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

int _sz[POA];       /* Sizes of attributes */
int _VT[PO][POA];   /* Virtual tables      */

#define _SZ_INT sizeof(int)
#define _SZ_PTR sizeof(void *)
#define _SZ_DBL sizeof(double)
#define _SZ_CH8 sizeof(char[8])

void init_method_tags(void) {
    _sz[POA_none] = 0;
    _sz[POA_type] = _SZ_INT;
    _sz[POA_prev] = _SZ_PTR;
    _sz[POA_RA] = _SZ_DBL;
    _sz[POA_pmRA] = _SZ_DBL;
    _sz[POA_DE] = _SZ_DBL;
    _sz[POA_pmDE] = _SZ_DBL;
    _sz[POA_PX] = _SZ_DBL;
    _sz[POA_RV] = _SZ_DBL;
    _sz[POA_V] = _SZ_DBL;
    _sz[POA_V_max] = _SZ_DBL;
    _sz[POA_V_min] = _SZ_DBL;
    _sz[POA_diam] = _SZ_DBL;
    _sz[POA_ecc] = _SZ_DBL;
    _sz[POA_angsep] = _SZ_DBL;
    _sz[POA_posang] = _SZ_DBL;
    _sz[POA_magdiff] = _SZ_DBL;
    _sz[POA_SP] = _SZ_CH8;
    _sz[POA_PN_morph] = _SZ_CH8;
    _sz[POA_gxy_morph] = _SZ_CH8;
    _sz[POA_neb_class] = _SZ_CH8;
    _sz[POA_subobjs] = _SZ_INT;
    _sz[POA_desg] = _SZ_CH8;
    _sz[POA_HIP] = _SZ_INT;
    _sz[POA_HD] = _SZ_INT;
}

void init_class(int type, int tag[]) {
    int ix;
    for (ix = 0; ix < POA; ix++) {
        _VT[type][ix] = -1;
    }
    _VT[type][POA_type] = 0;        /* _SZ_INT type attrib ALWAYS on zeroth index */
    _VT[type][POA_prev] = _SZ_INT;  /* _SZ_PTR prev attrib ALWAYS after */
    _VT[type][tag[0]] = _SZ_PTR;    /* then first attrib in list ... */
    for (ix = 1; tag[ix] != POA_none; ix++) { /* ... and so on */
        _VT[type][tag[ix]] = _VT[type][tag[ix-1]] + _sz[tag[ix-1]];
    }
    /* The total size MUST be put into attrib POA_size, */
    /*     or else ALL HELL breaks loose: */
    _VT[type][POA_size] = _VT[type][tag[ix-1]] + _sz[tag[ix-1]];
}

int ustring_to_attrib(uchar *ustr) {
    if (!ustr) return POA_none;

    /*==== POSITION AND APPARENT MOVEMENT: */
    /*---- Right Ascension */
    if (0 == ucscmp(ustr, u"RA")) return POA_RA;
    /*---- Proper Motion Right Ascension */
    if (0 == ucscmp(ustr, u"pmRA")) return POA_pmRA;
    /*---- Declination */
    if (0 == ucscmp(ustr, u"DE")) return POA_DE;
    /*---- Proper Motion Declination */
    if (0 == ucscmp(ustr, u"pmDE")) return POA_pmDE;

    /*==== DISTANCE AND DISTANCE CHANGE: */
    /*---- Parallax */
    if (0 == ucscmp(ustr, u"px")) return POA_PX;
    /*---- Radial Velocity */
    if (0 == ucscmp(ustr, u"RV")) return POA_RV;

    /*==== BRIGHTNESS: */
    /*---- Visual Magnitude */
    if (0 == ucscmp(ustr, u"V")) return POA_V;
    /*---- Max Visual Magnitude */
    if (0 == ucscmp(ustr, u"Vmax")) return POA_V_max;
    /*---- Min Visual Magnitude */
    if (0 == ucscmp(ustr, u"Vmin")) return POA_V_min;

    /*==== DEEP SKY METRICS: */
    /*---- Diameter (nebula size, apparent orbit etc.) */
    if (0 == ucscmp(ustr, u"diam")) return POA_diam;
    /*---- Eccentricity */
    if (0 == ucscmp(ustr, u"ecc")) return POA_ecc;
    /*---- Inclination */
    if (0 == ucscmp(ustr, u"incl")) return POA_incl;

    /*==== DOUBLE STAR METRICS: */
    /*---- Angular separation */
    if (0 == ucscmp(ustr, u"rho")) return POA_angsep;
    /*---- Position angle */
    if (0 == ucscmp(ustr, u"theta")) return POA_posang;

    /*==== ASTROPHYSICS */
    /*---- Spectrum */
    if (0 == ucscmp(ustr, u"SP")) return POA_SP;

    /** ...INSERTME(PN_morph, gxy_morph, neb_class)... */

    /*==== NAMES AND DESIGNATIONS */
    /*---- Bayer/Flamsteed designation */
    if (0 == ucscmp(ustr, u"Bayer")) return POA_desg;
    if (0 == ucscmp(ustr, u"desg")) return POA_desg;
    /*---- Hipparcos number */
    if (0 == ucscmp(ustr, u"HIP")) return POA_HIP;
    /*---- Henry Draper number */
    if (0 == ucscmp(ustr, u"HD")) return POA_HD;

    /* No such parameter! */
    return POA_none;
}

void init_star_class_old(void) {
    int taglist[] = {
        POA_RA, POA_DE, POA_V, POA_HIP, POA_none
    };
    init_class(PO_STAR, taglist);
}

void init_star_class(void) {
    int taglist[40];
    uchar *utaglist[] = {
        u"RA", u"DE", u"V", u"HIP", 0
    };
    int ix;
    for (ix = 0; utaglist[ix]; ix++)
        taglist[ix] = ustring_to_attrib(utaglist[ix]);
    taglist[ix] = POA_none;
    init_class(PO_STAR, taglist);
}

void init_named_class(int object_class, uchar **utags) {
    int taglist[40];
    int ix;
    for (ix = 0; utags[ix]; ix++)
        taglist[ix] = ustring_to_attrib(utags[ix]);
    taglist[ix] = POA_none;
    init_class(object_class, taglist);
}

pointobj *new_obj(int type, double RA, double DE, double V, pointobj *prev) {
    byte *res = (byte *)malloc(_VT[PO_STAR][POA_size]);
    _iset(res, 0, type);
    _pset(res, _VT[type][POA_prev], prev);
    _dset(res, _VT[type][POA_RA], RA);
    _dset(res, _VT[type][POA_DE], DE);
    _dset(res, _VT[type][POA_V], V);
    return (pointobj *)res;
}

int obj_type(pointobj *obj) {
    return _iget(obj, 0);
}

void init_classes(void) {
    init_method_tags();
    init_star_class();
}

pointobj *new_pointobj(
    int type, int HIP, double RA, double DE, double V, pointobj *prev
) {
    pointobj *res;
    switch (type) {
        case PO_STAR:
            res = new_obj(PO_STAR, RA, DE, V, prev);
            _iset((byte *)res, _VT[PO_STAR][POA_HIP], HIP);
            return res;
        default:
            return 0;
    }
    return res;
}

VIEW(pointobj) *new_pointobj_view(int size) {
    VIEW(pointobj) *res = ALLOC(VIEW(pointobj));
    res->size = size;
    res->next = 0;
    res->S = ALLOCN(pointobj *,size);
    return res;
}

VIEW(pointobj) *copy_pointobj_view(VIEW(pointobj) *POV) {
    VIEW(pointobj) *res = new_pointobj_view(POV->size);
    int ix;
    res->size = POV->size;
    res->next = POV->next;
    for (ix = 0; ix < POV->next; ix++) {
        res->S[ix] = POV->S[ix];
    }
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
    double V1 = _dget(S1,_VT[PO_STAR][POA_V]);
    double V2 = _dget(S2,_VT[PO_STAR][POA_V]);
    return (V1 > V2) - (V1 < V2);
}

int pointobj_cmp_by_HIP(const void *P1, const void *P2) {
    pointobj *S1 = *((pointobj **)P1);
    pointobj *S2 = *((pointobj **)P2);
    int H1 = _iget(S1,_VT[PO_STAR][POA_HIP]);
    int H2 = _iget(S2,_VT[PO_STAR][POA_HIP]);
    return (H1 > H2) - (H1 < H2);
}

void dump_pointobjs(FILE *stream, pointobj *S) {
    if (obj_type(S) == PO_STAR) {
        fprintf(stream, "  (star: HIP =% 7i, α = %12.8f, δ = %12.8f, m = %4.2f)\n",
                _iget(S, _VT[PO_STAR][POA_HIP]),
                _dget(S, _VT[PO_STAR][POA_RA]),
                _dget(S, _VT[PO_STAR][POA_DE]),
                _dget(S, _VT[PO_STAR][POA_V]));
    }
    else {
        fprintf(stream, "  (undescribable object)\n");
    }
}

void dump_pointobj_view(FILE *stream, VIEW(pointobj) *SV) {
    int ix;
    fprintf(stream, "stars_by_mag: size=%i, alloc=%i {\n", SV->next, SV->size);
    for(ix = 0; ix < SV->next; ix++) {
        dump_pointobjs(stream, SV->S[ix]);
    }
    fprintf(stream, "}\n");
}

double pointobj_attr_D(pointobj *pobj, int attr) {
    if (_sz[attr] == _SZ_DBL) {
        return _dget(pobj,_VT[obj_type(pobj)][attr]);
    }
    return -1;
}

int pointobj_attr_I(pointobj *pobj, int attr) {
    if (_sz[attr] == _SZ_INT) {
        return _iget(pobj,_VT[obj_type(pobj)][attr]);
    }
    return -1;
}

