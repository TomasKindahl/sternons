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

/* STANDARD LIBS */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* UTILS */
#include "defs.h"
#include "mathx.h"
#include "usio.h"
#include "ucstr.h"
#include "fields.h"

/* DRAWING OBJECTS */
#include "pointobj.h"
#include "line.h"
#include "polygon.h"
#include "label.h"
#include "projection.h"
#include "image.h"

/* VM ITEMS */
#include "valstack.h"
#include "progstat.h"

/* BINDING CODE */
#include "ps_db.h"

void sort_pointobj_view(progstat *pstat, VIEW(pointobj) *view,
                        int compare(const void *, const void *))
{
    /* quicksort the point objects in 'view' by function 'compare',
       currently the 'compares' used reside in pointobj.[ch] */
    qsort((void *)view->S, view->next, sizeof(pointobj *), compare);
}

int load_stars(progstat *pstat, char *fname) {
    /*
    DROP TABLE _cmap;
    SELECT hip, ra, de, vmag, _bv, _hvartype, _multflag, _sptype into _cmap 
           from _hipp where vmag < 6.5 order by vmag, ra, de;
    COPY _cmap TO '/home/rursus/Desktop/dumps/sternons/trunk/star.db'
         DELIMITER '|';

    -- MUST CREATE FULL SQL CONVERTERS FROM ORIGINAL HIPPARCHOS FILE TO
    -- MKMAP PREPARED FILES, INSTEAD OF THESE NOTES!!
    */
    int HIP;
    double RA, DE, vmag;
    uchar line[1024], *pos;
    utf8_file *inf = u8fopen(fname, "rt");

    if (!inf) return 0;
    /* LOAD THE STARS */
    while (fgetus(line, 1023, inf)) {
        line[ucslen(line)-1] = L'\0';
        pos = line;
        HIP = ucstoi(pos);
        RA = next_field_double(&pos);
        DE = next_field_double(&pos);
        vmag = next_field_double(&pos);
        pstat->latest_star =
            new_pointobj(PO_STAR, HIP, RA, DE, vmag, pstat->latest_star);
        append_pointobj(pstat->stars[BY_VMAG], pstat->latest_star);
        append_pointobj(pstat->stars[BY_HIP], pstat->latest_star);
    }
    /* sort stars by visual magnitude */
    sort_pointobj_view(pstat, pstat->stars[BY_VMAG], pointobj_cmp_by_V);
    /* sort stars by HIP number */
    sort_pointobj_view(pstat, pstat->stars[BY_HIP], pointobj_cmp_by_HIP);
    u8fclose(inf);
    return 1;
}

pointobj *find_star_by_HIP(int HIP, progstat *pstat) {
    pointobj *s, *res;

    s = new_pointobj(PO_STAR, HIP, 0, 0, 0, 0);
    res = *((pointobj **)bsearch(&s, (void *)pstat->stars[BY_HIP]->S,
                                  pstat->stars[BY_HIP]->next,
                                  sizeof(pointobj *), pointobj_cmp_by_HIP));
    free(s);
    return res;
}

int load_star_lines(progstat *pstat, char *fname) {
    int boldness, HIP_0, HIP_1;
    double RA_0, DE_0;
    double RA_1, DE_1;
    uchar line[1024], *pos, *asterism;
    utf8_file *inf = u8fopen(fname, "rt");

    if (!inf) return 0;
    while (fgetus(line, 1023, inf)) {
        /* char buf[128]; */
        pointobj *S;
        line[ucslen(line)-1] = L'\0';
        pos = line;
        boldness = ucstoi(pos);
        asterism = next_field_ustr(&pos);
        HIP_0 = next_field_int(&pos);
        S = find_star_by_HIP(HIP_0, pstat);
        if (S) {
            RA_0 = pointobj_attr_D(S, POA_RA);
            DE_0 = pointobj_attr_D(S, POA_DE); /* vmag1 = S->vmag; */
        }
        else {
            RA_0 = 666; DE_0 = 666; /* vmag1 = 666; */
        }
        HIP_1 = next_field_double(&pos);
        S = find_star_by_HIP(HIP_1, pstat);
        if (S) {
            RA_1 = pointobj_attr_D(S, POA_RA); 
            DE_1 = pointobj_attr_D(S, POA_DE); /* vmag2 = S->vmag; */
        }
        else {
            RA_1 = 666; DE_1 = 666; /* vmag2 = 666; */
        }
        if(0) { char buf[256];
            fprintf(stdout, "line %s %s:\n", 
                    boldness==1?"heavy":(boldness==2?"bold":"light"),
                    ucstombs(buf,asterism,128));
            fprintf(stdout, "  star 1 HIP=%i RA=%f DE=%f\n", HIP_0, RA_0, DE_0);
            fprintf(stdout, "  star 2 HIP=%i RA=%f DE=%f\n", HIP_1, RA_1, DE_1);
        }
        pstat->latest_line = new_line(boldness, asterism, HIP_0, HIP_1,
                                      pstat->latest_line);
        line_set_pos(pstat->latest_line, 0, RA_0, DE_0);
        line_set_pos(pstat->latest_line, 1, RA_1, DE_1);
    }
    u8fclose(inf);
    return 1;
}

int load_constellation_bounds(progstat *pstat, char *fname) {
    int boldness;
    double RA_0, DE_0;
    double RA_1, DE_1;
    uchar line[1024], *pos, *constel;
    utf8_file *inf = u8fopen(fname, "rt");

    if (!inf) return 0;
    while (fgetus(line, 1023, inf)) {
        /* char buf[128]; */
        line[ucslen(line)-1] = L'\0';
        pos = line;
        boldness = ucstoi(pos);
        constel = next_field_ustr(&pos);
        RA_0 = next_field_double(&pos)*15;
        DE_0 = next_field_double(&pos);
        RA_1 = next_field_double(&pos)*15;
        DE_1 = next_field_double(&pos);
        if (0) { char buf[256];
            fprintf(stdout, "border %s:\n", ucstombs(buf,constel,128));
            fprintf(stdout, "  start RA=%f DE=%f\n", RA_0, DE_0);
            fprintf(stdout, "  end RA=%f DE=%f\n", RA_1, DE_1);
        }
        /* as lines */
        pstat->latest_bound = new_line(4, constel, -1, -1, pstat->latest_bound);
        line_set_pos(pstat->latest_bound, 0, RA_0, DE_0);
        line_set_pos(pstat->latest_bound, 1, RA_1, DE_1);
        /* as polygonal areas */
        pstat->last_const = add_point(constel, RA_0, DE_0, pstat->last_const);
        if (!pstat->first_const) {
            pstat->first_const = pstat->last_const;
        }
        /** cleanup constel */
    }
    u8fclose(inf);
    /* dump_polygon_set(pstat->first_const); */
    return 1;
}

int load_star_labels(progstat *pstat, char *fname) {
    int HIP, anchor;
    double distance, direction, RA, DE;
    uchar line[1024], *pos, *label;
    utf8_file *inf = u8fopen(fname, "rt");

    while (fgetus(line, 1023, inf)) {
        pointobj *S;
        line[ucslen(line)-1] = L'\0';
        pos = line;
        HIP = ucstoi(pos);
        label = next_field_ustr(&pos);
        distance = next_field_double(&pos);
        direction = next_field_double(&pos);
        anchor = next_field_int(&pos);
        pstat->latest_star_label = new_label(HIP, label, distance, direction,
                                             anchor, pstat->latest_star_label);
        S = find_star_by_HIP(HIP, pstat);
        if (S) {
            double dir = deg2rad(direction);
            RA = pointobj_attr_D(S,POA_RA) + distance * cos(dir);
            DE = pointobj_attr_D(S,POA_DE) + distance * sin(dir);
        }
        else {
            RA = 666; DE = 666;
        }
        label_set_pos(pstat->latest_star_label, RA, DE);
    }
    u8fclose(inf);

    return 1;
}


