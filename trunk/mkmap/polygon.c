/************************************************************************
 *  Copyright (C) 2010-2012 Tomas Kindahl                               *
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
#include "usio.h"
#include "ucstr.h"
#include "polygon.h"

polygon *new_polygon(double RA, double DE, polygon *prev) {
    polygon *res = ALLOC(polygon);
    res->RA = RA;
    res->DE = DE;
    res->prev = prev;
    return res;
}

polygon_set *new_polygon_set(uchar *name, polygon_set *next) {
    polygon_set *res = ALLOC(polygon_set);
    res->name = ucsdup(name);
    res->poly = 0;
    res->next = next;
    return res;
}

polygon_set *add_point(uchar *name, double RA, double DE, polygon_set *current) {
    polygon_set *res;
    if (!current) {
        res = new_polygon_set(name, 0);
    }
    else if (0 != ucscmp(name, current->name)) {
        res = new_polygon_set(name, 0);
        current->next = res;
    }
    else {
        res = current;
    }
    res->poly = new_polygon(RA, DE, res->poly);
    return res;
}

void dump_polygon_set(polygon_set *PS) {
    char buf[256];
    polygon *P;
    int ix;
    
    if (!PS) return;
    printf("polygon set for %s: ", ucstombs(buf, PS->name, 256));
    for (P = PS->poly, ix = 0; P; P = P->prev, ix++);
    printf("%i\n", ix);
    dump_polygon_set(PS->next);
}

