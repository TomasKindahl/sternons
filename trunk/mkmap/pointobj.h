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

#ifndef _POINTOBJ_H
#define _POINTOBJ_H

#include "meta.h"

#define PO_ANY                 0x0000
#define PO_STAR                0x1000
#define PO_VARIABLE_STAR       0x1100
#define PO_COMPOSITE           0x2000
#define PO_MULTIPLE_STAR       0x2100
#define PO_ASTROMETRIC_VAR     0x2110
#define PO_ECLIPSING_VAR       0x2120
#define PO_CATACLYSMIC_VAR     0x2130
#define PO_STAR_CLUSTER        0x2200
#define PO_OPEN_CLUSTER        0x2210
#define PO_GLOBULAR_CLUSTER    0x2220
#define PO_STAR_CLOUD          0x2300
#define PO_STAR_ASSOCIATION    0x2400
#define PO_GALAXY              0x2500
#define PO_NEBULA              0x3000
#define PO_STAR_EJECTA         0x3100
#define PO_PLANETARY_NEBULA    0x3110
#define PO_WOLF_RAYET_NEBULA   0x3120
#define PO_SUPERNOVA_REMNANT   0x3130
#define PO_BRIGHT_NEBULA       0x3200
#define PO_EMISSION_NEBULA     0x3210
#define PO_REFLECTION_NEBULA   0x3220
#define PO_DARK_NEBULA         0x3300
#define PO_SOLAR_SYSTEM_OBJECT 0x4000
#define PO_SUN                 0x4100
#define PO_PLANETOID           0x4200
#define PO_COMET               0x4210
#define PO_METEOR_SWARM        0x4300

/** PREDEFINED ATTRIBUTES: **/
#define POA_none        0  /* for unrecognized attributes     */
#define POA_size        1  /* INT size of object of class     */
#define POA_TYPE        2  /* INT type tag of class           */
#define POA_prev        3  /* PTR previous same               */
#define POA_RA          4  /* REAL right ascension            */
#define POA_RA_delta    5  /* REAL right ascension change     */
#define POA_DE          6  /* REAL declination                */
#define POA_DE_delta    7  /* REAL declination change         */
#define POA_PX          8  /* REAL parallax                   */
#define POA_RV          9  /* REAL radial velocity            */
#define POA_V          10  /* REAL visual magnitude           */
#define POA_V_max      11  /* REAL maximum visual magnitude   */
#define POA_V_min      12  /* REAL minimum visual magnitude   */
#define POA_diam       13  /* REAL diameter                   */
#define POA_ell        14  /* REAL object ellipticity         */
#define POA_aspect     15  /* REAL ellipse angle              */
#define POA_SP         16  /* STR spectrum                    */
#define POA_PN_morph   17  /* STR PN morphology               */
#define POA_gxy_morph  18  /* STR gxy morphology              */
#define POA_neb_class  19  /* STR nebula class                */
#define POA_subobjs    20  /* INT number of object components */
#define POA_desg       21  /* STR Bayer designation           */
#define POA_HIP        22  /* INT Hipparcos catalog number    */
#define POA_HD         23  /* INT Henry Draper catalog        */
#define POA            24  /* number of parameters            */

typedef struct _cpointobj_T {
	struct _cpointobj_VT *VT;
} cpointobj;

typedef struct _pointobj_T {
	int type;
    double RA, DE;
    double V;
    int HIP; /* 2mv */
    struct _pointobj_T *prev;
    /* 2Bdef: int cat; -- catalogue */
} pointobj;

DEFVIEW(pointobj);

void init_classes(void);
pointobj *new_pointobj(int type, int HIP, double RA, double DE, double vmag, pointobj *prev);
pointobj_view *new_pointobj_view(int size);
int append_pointobj(pointobj_view *SV, pointobj *S);
int pointobj_cmp_by_V(const void *P1, const void *P2);
int pointobj_cmp_by_HIP(const void *P1, const void *P2);
void dump_pointobjs(FILE *stream, pointobj *S);
void dump_pointobj_view(FILE *stream, pointobj_view *SV);
double pointobj_attr_D(pointobj *S, int attr);
int pointobj_attr_I(pointobj *S, int attr);

#endif /* _POINTOBJ_H */

