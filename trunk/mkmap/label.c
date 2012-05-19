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
#include "meta.h"
#include "defs.h"
#include "ucstr.h"
#include "label.h"

label *new_label(int HIP, uchar *text, double distance, double direction,
                 int anchor, label *prev)
{
    label *res = ALLOC(label);
    res->HIP = HIP;
    res->text = ucsdup(text);
    res->distance = distance;
    res->direction = direction;
    res->RA = -999;
    res->DE = -999;
    res->anchor = anchor;
    res->prev = prev;
    return res;
}

label *label_set_pos(label *L, double RA, double DE) {
    if (!L) return 0;
    L->RA = RA; L->DE = DE;
    return L;
}

