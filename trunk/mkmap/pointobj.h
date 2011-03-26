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

#define CLASS_STAR              0x100
#define CLASS_OPEN_CLUSTER      0x200
#define CLASS_GLOBULAR_CLUSTER  0x300
#define CLASS_PLANETARY_NEBULA  0x400
#define CLASS_BRIGHT_NEBULA     0x500
#define CLASS_DARK_NEBULA       0x600
#define CLASS_GALAXY            0x700

typedef struct _pointobj_T {
    int type;
    double RA, DE, vmag;
    struct _pointobj_T *prev;
    /* 2Bdef: int cat; -- catalogue */
    int HIP; /* 2mv */
} pointobj;

DEFVIEW(pointobj);

pointobj *new_pointobj(int type, int HIP, double RA, double DE, double vmag, pointobj *prev);
pointobj_view *new_pointobj_view(int size);
int append_pointobj(pointobj_view *SV, pointobj *S);
int pointobj_cmp_by_vmag(const void *P1, const void *P2);
int pointobj_cmp_by_HIP(const void *P1, const void *P2);
void dump_pointobjs(FILE *stream, pointobj *S);
void dump_pointobj_view(FILE *stream, pointobj_view *SV);

#endif /* _POINTOBJ_H */

