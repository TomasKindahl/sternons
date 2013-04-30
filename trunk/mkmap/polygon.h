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

#ifndef _POLYGON_H
#define _POLYGON_H

typedef struct _polygon_S {
    double RA;
    double DE;
    struct _polygon_S *prev;
} polygon;

polygon *new_polygon(double RA, double DE, polygon *prev);

typedef struct _polygon_set_S {
    polygon *poly;
    uchar *name;
    struct _polygon_set_S *next;
} polygon_set;

polygon_set *new_polygon_set(uchar *name, polygon_set *next);
polygon_set *add_point(uchar *name, double RA, double DE, polygon_set *current);
void dump_polygon_set(polygon_set *PS);

#endif /* _POLYGON_H */

