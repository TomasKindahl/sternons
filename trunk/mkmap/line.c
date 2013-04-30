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
#include "meta.h"
#include "defs.h"
#include "ucstr.h"
#include "line.h"

line *new_line(int boldness, uchar *asterism, int HIP_1, int HIP_2, line *prev) {
    line *res = ALLOC(line);
    res->boldness = boldness;
    res->asterism = ucsdup(asterism);
    res->HIP[0] = HIP_1; res->HIP[1] = HIP_2;
    res->prev = prev;
    return res;
}

void line_set_pos(line *L, int ix, double RA, double DE) {
    L->RA[ix] = RA; L->DE[ix] = DE;
}
