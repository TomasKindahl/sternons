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
#include <stdio.h>
#include <time.h>           /* For time stamping the imgs in debug mode */

#define NO_DEBUG 0
#define DEBUG 1

/* UTILS */
#include "defs.h"
#include "mathx.h"
#include "usio.h"
#include "ucstr.h"

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
#include "ps_draw.h"

#define FONT_STYLE "font-family: FreeSans, sans serif; font-weight: bold;"

int draw_head(progstat *pstat) {
    char buf[256];
    int H, W;
    image *img = pstat->img;
    FILE *out = pstat->out_file;

    W = img->width;
    H = img->height;
    fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\" "
                 "standalone=\"no\"?>\n");
    fprintf(out, "<svg width=\"%i\" height=\"%i\"\n"
                 "     xmlns=\"http://www.w3.org/2000/svg\"\n"
                 "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
                 "     >\n", W, H);
    fprintf(out, "     <title>%s</title>\n",
                 ucstombs(buf, pstat->img->name, 256));
	return 1;
}

int draw_background(progstat *pstat) {
    int ix, iy, H, W, H2, W2, dim;
    image *img = pstat->img;
    FILE *out = pstat->out_file;

    W = img->width; W2 = W/2;
    H = img->height; H2 = H/2;
    dim = img->dim;
    if (pstat->debug) fprintf(out, "    <!-- BACKGROUND -->\n");
    fprintf(out, "    <rect style=\"opacity:1;fill:#000033;fill-opacity:1;stroke:none;"
                 "stroke-width:0.2;stroke-linejoin:miter;stroke-miterlimit:4;"
                 "stroke-dasharray:none;stroke-opacity:1\"\n"
                 "          width=\"%i\" height=\"%i\"/>\n", W, H);
    if (0 && pstat->debug == DEBUG) {
        /* Helper lines, paper coordinates: */
        /* vertical */
        for (ix = 100; ix < 560; ix += 100) {
            fprintf(out, "    <path style=\"stroke:#000000;stroke-width:1px\" "
                         "d=\"M %i,0 L %i,%i\"/>\n", ix, ix, H);
            fprintf(out, "    <path style=\"stroke:#080808;stroke-width:1px\" "
                         "d=\"M %i,0 L %i,%i\"/>\n", ix-50, ix-50, H);
        }
        /* horizontal */
        for (iy = 100; iy < 560; iy += 100) {
            fprintf(out, "    <path style=\"stroke:#000000;stroke-width:1px\" "
                         "d=\"M 0,%i L %i,%i\"/>\n", iy, W, iy);
            fprintf(out, "    <path style=\"stroke:#080808;stroke-width:1px\" "
                         "d=\"M 0,%i L %i,%i\"/>\n", iy-50, W, iy-50);
        }
    }
	return 1;
}

int draw_grid(progstat *pstat) {
    int ix, iy, H, W, H2, W2, dim;
    double ra, ras[24], de, des[17];
    double X, Y;
    double x, y;
    image *img = pstat->img;
    proj *projection = img->projection;
    FILE *out = pstat->out_file;

    W = img->width; W2 = W/2;
    H = img->height; H2 = H/2;
    dim = img->dim;
    /* Declination lines: */
    #define NUM_DE 17

    fprintf(out, "    <!-- GRID -->\n");
    for (iy = 0; iy < NUM_DE; iy++) {
        des[iy] = deg2rad(iy*10-80);
    }
    for (ra = 0; ra < 360; ra+=0.5) {
        for (ix = 0; ix < NUM_DE; ix++) {
            project(&X, &Y, des[ix], deg2rad(ra), projection);
            if (IMG_relative_pos(&x, &y, X, Y, img)) {
                fprintf(out, "    <circle cx=\"%.2f\" cy=\"%.2f\" r=\"1\"\n", x, y);
                fprintf(out, "            style=\"opacity:1;fill:#880088;");
                fprintf(out, "fill-opacity:1;\"/>\n");
            }
        }
    }
    /* Right Ascension lines: */
    #define NUM_RA 24
    for (ix = 0; ix < NUM_RA; ix++) {
        ras[ix] = deg2rad(ix*15);
    }
    for (de = -80; de <= 80; de+=0.5) {
        for (iy = 0; iy < NUM_RA; iy++) {
            project(&X, &Y, deg2rad(de), ras[iy], projection);
            if (IMG_relative_pos(&x, &y, X, Y, img)) {
                fprintf(out, "    <circle cx=\"%.2f\" cy=\"%.2f\" r=\"1\"\n", x, y);
                fprintf(out, "            style=\"opacity:1;fill:#880088;");
                fprintf(out, "fill-opacity:1;\"/>\n");
            }
        }
    }
	return 1;
}

int draw_stars(progstat *pstat) {
    /*
    DROP TABLE _cmap;
    SELECT hip, ra, de, vmag, _bv, _hvartype, _multflag, _sptype into _cmap 
           from _hipp where vmag < 6.5 order by vmag, ra, de;
    COPY _cmap TO '/home/rursus/Desktop/dumps/sternons/trunk/star.db'
         DELIMITER '|';
    */
    int ix;
    int HIP;
    double RA, DE, vmag;
    double X, Y, size;
    double x, y;
    image *img = pstat->img;
    proj *projection = img->projection;
    FILE *out = pstat->out_file;
    pointobj *S = 0;

    /* DRAW THE STARS */
    if (pstat->debug) fprintf(out, "    <!-- STARS -->\n");
    for (ix = 0; ix < pstat->stars[BY_VMAG]->next; ix++) {
        S = pstat->stars[BY_VMAG]->S[ix];
        DE = pointobj_attr_D(S,POA_DE);
        RA = pointobj_attr_D(S,POA_RA);
        vmag = pointobj_attr_D(S,POA_V);
        HIP = pointobj_attr_I(S,POA_HIP);
        project(&X, &Y, deg2rad(DE), deg2rad(RA), projection);
        if(IMG_relative_pos(&x, &y, X, Y, img)) {
            size = (6.8-vmag)*0.8*img->scale;
            fprintf(out, "    <circle title=\"HIP %i\" cx=\"%.2f\" cy=\"%.2f\" r=\"%g\"\n",
                             HIP, x, y, size);
            fprintf(out, "            style=\"opacity:1;fill:#FFFFFF;fill-opacity:1;"
                         "stroke:#666666;stroke-width:1px\"/>\n");
        }
    }
    /* dump_stars_by_vmag_view(stderr, pstat->stars_by_vmag); */
	return 1;
}

int draw_line_set(progstat *pstat, uchar *id, line *line_set) {
    line *L;
    double X1, Y1, X2, Y2;
    double x1, y1, x2, y2;
    image *img = pstat->img;
    proj *projection = img->projection;
    FILE *out = pstat->out_file;
    char buf[256];

    for (L = line_set; L; L = L->prev) {
        if (id && 0 != ucscmp(id,L->asterism)) continue;
        project(&X1, &Y1, deg2rad(L->DE[0]), deg2rad(L->RA[0]), projection);
        project(&X2, &Y2, deg2rad(L->DE[1]), deg2rad(L->RA[1]), projection);
        if (IMG_relative_pos(&x1, &y1, X1, Y1, img)
          | IMG_relative_pos(&x2, &y2, X2, Y2, img)) {
            char *color;
            switch(L->boldness) {
                case 1: color = "0000FF"; break;
                case 2: color = "0000CC"; break;
                case 3: color = "0000AA"; break;
                case 4: color = "005577"; break; /* constellation borders */
                default: color = "220000"; break;
            }
            if (pstat->debug)
                fprintf(out, "    <!-- LINE FOR %s -->\n", ucstombs(buf, L->asterism, 256));
            fprintf(out, "    <path ");
            fprintf(out, "style=\"stroke:#%s; stroke-width: 1px\"\n", color);
            fprintf(out, "          d=\"M %.2f,%.2f L %.2f,%.2f\"/>\n",
                    x1, y1, x2, y2);
        }
    }
	return 1;
}

int draw_lines(progstat *pstat, uchar *id) {
    return draw_line_set(pstat, id, pstat->latest_line);
}

int draw_bounds(progstat *pstat) {
    return draw_line_set(pstat, 0, pstat->latest_bound);
}

int draw_delportian_area(progstat *pstat, uchar *id) {
    polygon_set *PS;
    image *img = pstat->img;
    proj *projection = img->projection;
    FILE *out = pstat->out_file;

    for (PS = pstat->first_const; PS; PS = PS->next) {
        if (0 == ucscmp(id, PS->name)) {
            double X, Y;
            double x, y;
            polygon *P = PS->poly;
            char buf[256];

            if (pstat->debug) 
                fprintf(out, "    <!-- bounds for %s -->\n", ucstombs(buf,PS->name,256));
            fprintf(out, "    <path ");
            fprintf(out, "style=\"stroke:#0088AA; stroke-width: 1px; ");
            fprintf(out, "fill: #000022\"\n");
            P = PS->poly;
            project(&X, &Y, deg2rad(P->DE), deg2rad(P->RA), projection);
            IMG_relative_pos(&x, &y, X, Y, img);
            fprintf(out, "          d=\"M %.2f,%.2f\n", x, y);
            for (P = P->prev; P; P = P->prev) {
                project(&X, &Y, deg2rad(P->DE), deg2rad(P->RA), projection);
                IMG_relative_pos(&x, &y, X, Y, img);
                fprintf(out, "             L %.2f,%.2f\n", x, y);
            }
            fprintf(out, "             Z\"\n");
            fprintf(out, "/>\n");
        }
    }
	return 1;
}

int draw_labels(progstat *pstat, uchar *NOTUSEDYET) {
    label *L;
    image *img = pstat->img;
    proj *projection = img->projection;
    FILE *out = pstat->out_file;

    for (L = pstat->latest_star_label; L; L = L->prev) {
        double X, Y;
        double x, y;
        char buf[256], *anchor;
        project(&X, &Y, deg2rad(L->DE), deg2rad(L->RA), projection);
        if (IMG_relative_pos(&x, &y, X, Y, img)) {
            switch (L->anchor) {
                  case 1: anchor = "start"; break;
                  case 2: anchor = "middle"; break;
                  case 3: anchor = "end"; break;
                  default: anchor = "end"; break;
            }
            if (pstat->debug) 
                fprintf(out, "    <!-- label \"%s\" for HIP %i -->\n",
                        ucstombs(buf,L->text,256),
                        L->HIP);
            fprintf(out, "    <text x=\"%.2f\" y=\"%.2f\" "
                         "style=\"fill:#CC0000; text-anchor: %s;"
                         "font-size:13px;%s\">%s</text>\n",
                    x, y, anchor, FONT_STYLE, ucstombs(buf,L->text,256));
        }
    }
	return 1;
}

int draw_debug_info(progstat *pstat) {
    struct tm *TM;
    time_t T = time(NULL);
    int height = pstat->img->height;

    if (pstat->debug == DEBUG) {
        T = time(NULL);
        if (T == -1) return 0;
        TM = localtime(&T);
        fprintf(pstat->out_file, "    <text x=\"%i\" y=\"%i\" "
                "style=\"fill:#00CC00;font-size:11px\">generated by mkmap "
                "%04i-%02i-%02i, %02i:%02i"
                "</text>\n",
                10, height - 10,
                TM->tm_year+1900, TM->tm_mon+1, TM->tm_mday,
                TM->tm_hour, TM->tm_min
               );
    }
	return 1;
}

int draw_foot(progstat *pstat) {
    /* printf("    <img y=\"200\" x=\"200\" height=\"100\" width=\"100\"\n"); */
    /* printf("           xlink:href=\"neptune.png\" />\n"); */
    fprintf(pstat->out_file, "</svg>\n");
	return 1;
}

int close_file(progstat *pstat) {
    return fclose(pstat->out_file);
}

