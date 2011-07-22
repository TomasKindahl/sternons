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

#define PO_ANY                 0
#define PO_STAR                1
#define PO_VARIABLE_STAR       2
#define PO_COMPOSITE           3
#define PO_MULTIPLE_STAR       4
#define PO_ASTROMETRIC_VAR     5
#define PO_ECLIPSING_VAR       6
#define PO_CATACLYSMIC_VAR     7
#define PO_STAR_CLUSTER        8
#define PO_OPEN_CLUSTER        9
#define PO_GLOBULAR_CLUSTER    10
#define PO_STAR_CLOUD          11
#define PO_STAR_ASSOCIATION    12
#define PO_GALAXY              13
#define PO_NEBULA              14
#define PO_STAR_EJECTA         15
#define PO_PLANETARY_NEBULA    16
#define PO_WOLF_RAYET_NEBULA   17
#define PO_SUPERNOVA_REMNANT   18
#define PO_BRIGHT_NEBULA       19
#define PO_EMISSION_NEBULA     20
#define PO_REFLECTION_NEBULA   21
#define PO_DARK_NEBULA         22
#define PO_SOLAR_SYSTEM_OBJECT 23
#define PO_SUN                 24
#define PO_PLANETOID           25
#define PO_COMET               26
#define PO_METEOR_SWARM        27
#define PO                     28

/** PREDEFINED ATTRIBUTES: **/
#define POA_none        0  /* for unrecognized attributes     */
#define POA_size        1  /* INT size of object of class     */
#define POA_prev        2  /* PTR previous same               */
#define POA_RA          3  /* REAL right ascension            */
#define POA_RA_delta    4  /* REAL right ascension change     */
#define POA_DE          5  /* REAL declination                */
#define POA_DE_delta    6  /* REAL declination change         */
#define POA_PX          7  /* REAL parallax                   */
#define POA_RV          8  /* REAL radial velocity            */
#define POA_V           9  /* REAL visual magnitude           */
#define POA_V_max      10  /* REAL maximum visual magnitude   */
#define POA_V_min      11  /* REAL minimum visual magnitude   */
#define POA_diam       12  /* REAL diameter                   */
#define POA_ell        13  /* REAL object ellipticity         */
#define POA_aspect     14  /* REAL ellipse angle              */
#define POA_SP         15  /* STR spectrum                    */
#define POA_PN_morph   16  /* STR PN morphology               */
#define POA_gxy_morph  17  /* STR gxy morphology              */
#define POA_neb_class  18  /* STR nebula class                */
#define POA_subobjs    19  /* INT number of object components */
#define POA_desg       20  /* STR Bayer designation           */
#define POA_HIP        21  /* INT Hipparcos catalog number    */
#define POA_HD         22  /* INT Henry Draper catalog        */
#define POA            23  /* number of parameters            */

typedef struct _cpointobj_S {
	struct _cpointobj_VT *VT;
} cpointobj;

typedef struct _pointobj_S {
	int dum;
} pointobj;

DEFVIEW(pointobj);

void init_classes(void);
pointobj *new_pointobj(
    int type, int HIP, double RA, double DE, double vmag, pointobj *prev
);
pointobj_view *new_pointobj_view(int size);
int append_pointobj(pointobj_view *SV, pointobj *S);
int pointobj_cmp_by_V(const void *P1, const void *P2);
int pointobj_cmp_by_HIP(const void *P1, const void *P2);
void dump_pointobjs(FILE *stream, pointobj *S);
void dump_pointobj_view(FILE *stream, pointobj_view *SV);
int obj_type(pointobj *obj);
double pointobj_attr_D(pointobj *S, int attr);
int pointobj_attr_I(pointobj *S, int attr);

#endif /* _POINTOBJ_H */

