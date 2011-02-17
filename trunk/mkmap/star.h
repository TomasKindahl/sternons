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

#ifndef _STAR_H
#define _STAR_H

typedef struct _star_T {
    /* TBA: int type; -- class */
    double RA, DE, vmag;
    struct _star_T *prev;
    int HIP;
} star;

typedef struct _star_view_S {
    int size;
    int next;
    star **S;
} star_view;

star *new_star(star *prev, int HIP, double RA, double DE, double vmag);
star_view *new_star_view(int size);
int append_star(star_view *SV, star *S);
int star_cmp_by_vmag(const void *P1, const void *P2);
int star_cmp_by_HIP(const void *P1, const void *P2);
void dump_stars(FILE *stream, star *S);
void dump_stars_view(FILE *stream, star_view *SV);

#endif /* _STAR_H */

