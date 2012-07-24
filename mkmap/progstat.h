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

#ifndef _PROGSTAT_H
#define _PROGSTAT_H

#define BY_VMAG 0
#define BY_HIP 1

#include "valstack.h"

typedef struct _progstat_S {
    /* hard-coded feature data */
        /* physical objects */
            pointobj *latest_star;
            VIEW(pointobj) **stars; /* sorted lists pointing to individual */
                                     /* latest_star:s */
        /* virtual objects */
            /* asterism lines */
            line *latest_line;
            /* delportian constellation bounds lines */
            /** TBA: line_view *lines_by_label;       */
            line *latest_bound;
            /* delportian constellation areas */
            polygon_set *first_const;
            polygon_set *last_const;
            label *latest_star_label;
    /* hard-coded img data */
    image *img;
    FILE *out_file;
    /* program state data */
    int debug;
    valstack *vstack;
    struct _progstat_S *prev;
} progstat;

progstat *new_progstat(int debug, FILE *outerr);
progstat *PS_push(progstat *PS, FILE *outerr);
progstat *PS_pop(progstat *PS, FILE *outerr);
image *PS_set_img(progstat *PS, image *img);
image *PS_new_image(progstat *PS);
image *PS_img_set_name(progstat *PS, uchar *uname);
image *PS_img_set_size(progstat *PS, int width, int height);
image *PS_img_set_scale(progstat *PS, double scale);
image *PS_img_set_Lambert
    (progstat *PS, double l0, double p0, double p1, double p2);

#define PS_STACKDEFS(TYPE,CTYPE) \
    CTYPE PS_pop_ ## TYPE(progstat *PS); \
    void PS_push_ ## TYPE(progstat *PS, CTYPE int_val);
PS_STACKDEFS(int,int)
PS_STACKDEFS(dbl,double)
PS_STACKDEFS(ustr,uchar *)
PS_STACKDEFS(cstr,char *)

int PS_open_file(char *fname, progstat *PS);

#endif /* _PROGSTAT_H */
