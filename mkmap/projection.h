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

#ifndef _PROJECTION_H
#define _PROJECTION_H

#define PROJ_NONE 0
#define PROJ_POLAR 1
#define PROJ_CYLINDRIC 2
#define PROJ_LAMBERT_LCC 3

typedef struct _proj_S {
    int proj_type;
} proj;

proj *init_Lambert(double l0, double p0, double p1, double p2);
void project(double *x, double *y, double phi, double lambda, proj *P);

#endif /* _PROJECTION_H */
