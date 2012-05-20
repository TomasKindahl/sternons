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
#include "pointobj.h"
#include "line.h"
#include "polygon.h"
#include "label.h"
#include "projection.h"
#include "image.h"
#include "valstack.h"
#include "progstat.h"

progstat *new_progstat(int debug, FILE *outerr) {
    progstat *res = ALLOC(progstat);

    res->latest_star = 0;
    res->stars = ALLOCN(VIEW(pointobj) *,2);
    res->stars[BY_VMAG] = new_pointobj_view(1024);
    res->stars[BY_HIP] = new_pointobj_view(1024);
    /* asterism lines */
        res->latest_line = 0;
        res->latest_bound = 0;
    /* delportian areas */
        res->first_const = 0;
        res->last_const = 0;
    /* labels */
        res->latest_star_label = 0;
    /* img var */
        res->img = 0;
        res->out_file = 0;
    /* program state */
        res->debug = debug;
        res->vstack = 0;
        res->prev = 0;
    return res;
}

progstat *PS_push(progstat *PS, FILE *outerr) {
    progstat * res = new_progstat(PS->debug, outerr);
    res->latest_star = PS->latest_star;
    res->debug = PS->debug;
    res->stars = ALLOCN(VIEW(pointobj) *,2);
    res->stars[BY_VMAG] = copy_pointobj_view(PS->stars[BY_VMAG]);
    res->stars[BY_HIP] = copy_pointobj_view(PS->stars[BY_HIP]);
    /* asterism lines */
        res->latest_line = PS->latest_line;
        res->latest_bound = PS->latest_bound;
    /* delportian areas */
        res->first_const = PS->first_const;
        res->last_const = PS->last_const;
    /* labels */
        res->latest_star_label = PS->latest_star_label;
    /* img var */
        res->img = PS->img;
        res->out_file = PS->out_file;
    /* program state */
        res->debug = PS->debug;
        res->vstack = 0;
        res->prev = PS;
    return res;
}

progstat *PS_pop(progstat *PS, FILE *outerr) {
    /** FREE PS! and relevant contents **/
    return PS->prev;
}

image *PS_set_img(progstat *PS, image *img) {
    PS->img = img;
    return img;
}

image *PS_new_image
    (progstat *PS, uchar *name, int width, int height, double scale)
{
    image *img = new_image(name, width, height, scale);
    return PS_set_img(PS, img);
}

image *PS_img_set_Lambert
    (progstat *PS, double l0, double p0, double p1, double p2)
{
    proj *projection = init_Lambert(l0, p0, p1, p2);
    return IMG_set_projection(PS->img, projection);
}

void _PS_check_stack_empty(progstat *PS) {
    if(PS->vstack == 0) {
        fprintf(stderr, "FATAL: vstack underflow\n");
        exit(-1);
    }
}

#define _PS_STACKDEFS(TYPE,CTYPE) \
    CTYPE PS_pop_ ## TYPE(progstat *PS) { \
        CTYPE res; \
        _PS_check_stack_empty(PS); \
        res = VS_ ## TYPE(PS->vstack); \
        PS->vstack = VS_pop(PS->vstack); \
        return res; \
    } \
    void PS_push_ ## TYPE(progstat *PS, CTYPE TYPE ## _val) { \
        PS->vstack = VS_push_ ## TYPE(TYPE ## _val, PS->vstack); \
    }
_PS_STACKDEFS(int,int)
_PS_STACKDEFS(dbl,double)
_PS_STACKDEFS(ustr,uchar *)
_PS_STACKDEFS(cstr,char *)

int PS_open_file(char *fname, progstat *PS) {
    PS->out_file = fopen(fname, "wt");
    if (!PS->out_file) return 0;
    return 1;
}

