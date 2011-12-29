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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>           /* For time stamping the images in debug mode */
/* General program defs */
#include "defs.h"
/* Uchar literal conversion */
#include "allstrings.h"     /* generated strings */
/* General libraries */
#include "usio.h"
#include "ucstr.h"
#include "token.h"
/* Application libraries */
/* TD:  #include "parse.h"*/
#include "pointobj.h"       /* star struct and DB views */

#define NO_DEBUG 0
#define DEBUG 1

#include "mathx.h"
#include "projection.h"

typedef struct _image_struct_S {
    uchar *name;
    int width, height, dim;
    double scale;
    /* INSERTME: */
        /* colorings, magnitude limits, magnitude scalings ... other */
    proj *projection;
} image_struct;

image_struct *new_image(uchar *name, int width, int height, double scale) {
    image_struct *res = ALLOC(image_struct);
    res->name = ucsdup(name);
    res->width = width;
    res->height = height;
    if(width > height) res->dim = width; else res->dim = height;
    res->scale = scale;
    return res;
}

image_struct *image_set_projection(image_struct *image, proj *LCCP) {
    image->projection = LCCP;
    return image;
}

typedef struct _line_S {
    int boldness;
    uchar *asterism;
    int HIP_1; double RA_1, DE_1;
    int HIP_2; double RA_2, DE_2;
    struct _line_S *prev;
} line;

line *new_line(int boldness, uchar *asterism, int HIP_1, int HIP_2, line *prev) {
    line *res = ALLOC(line);
    res->boldness = boldness;
    res->asterism = ucsdup(asterism);
    res->HIP_1 = HIP_1; res->HIP_2 = HIP_2;
    res->prev = prev;
    return res;
}

#define LINE_SET_POS(IX) \
    void line_set_pos_ ## IX (line *L, double RA, double DE) { \
        L->RA_ ## IX = RA; L->DE_ ## IX = DE; \
    }

LINE_SET_POS(1)
LINE_SET_POS(2)

typedef struct _polygon_S {
    double RA;
    double DE;
    struct _polygon_S *prev;
} polygon;

polygon *new_polygon(double RA, double DE, polygon *prev) {
    polygon *res = ALLOC(polygon);
    res->RA = RA;
    res->DE = DE;
    res->prev = prev;
    return res;
}

typedef struct _polygon_set_S {
    polygon *poly;
    uchar *name;
    struct _polygon_set_S *next;
} polygon_set;

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

typedef struct _label_S {
    int HIP;
    uchar *text;
    double distance, direction, RA, DE;
    int anchor;
    struct _label_S *prev;
} label;

label *new_label(int HIP, uchar *text, double distance, double direction,
		         int anchor, label *prev) {
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

#define BY_VMAG 0
#define BY_HIP 1

typedef struct _program_state_S {
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
    /* hard-coded image data */
    image_struct *image;
    FILE *out_file;
    int debug;
    struct _program_state_S *prev;
} program_state;

program_state *new_program_state(int debug, FILE *outerr) {
    program_state *res = ALLOC(program_state);
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
	res->image = 0;
	res->out_file = 0;
    res->debug = debug;
	res->prev = 0;
    return res;
}

program_state *program_push(program_state *PS, FILE *outerr) {
    program_state * res = new_program_state(PS->debug, outerr);
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
	res->image = PS->image;
	res->out_file = PS->out_file;
    res->debug = PS->debug;
	res->prev = PS;
	return res;
}

program_state *program_pop(program_state *PS, FILE *outerr) {
	/** FREE PS! and relevant contents **/
	return PS->prev;
}

image_struct *program_set_image(program_state *prog, image_struct *image) {
    prog->image = image;
    return image;
}

int pos_in_frame(double *x, double *y, double X, double Y, image_struct *frame) {
    *x = frame->width/2-frame->dim*X*frame->scale;
    *y = frame->height/2-frame->dim*Y*frame->scale;
    return BETW(0,*x,frame->width) && BETW(0,*y,frame->height);
}

int open_file(char *fname, program_state *pstat) {
    pstat->out_file = fopen(fname, "wt");
    if (!pstat->out_file) return 0;
    return 1;
}

double next_field_double(uchar **pos) {
    *pos = ucschr(*pos,'|')+1;
    return ucstof(*pos);
}

int next_field_int(uchar **pos) {
    *pos = ucschr(*pos,'|')+1;
    return ucstoi(*pos);
}

uchar *next_field_ustr(uchar **pos) {
    uchar *pos2;

    *pos = ucschr(*pos,'|')+1;
    pos2 = ucschr(*pos,'|');
    return ucsndup(*pos,pos2-(*pos));
}

void sort_pointobj_view(program_state *pstat, VIEW(pointobj) *view,
						int compare(const void *, const void *))
{
    /* quicksort the point objects in 'view' by function 'compare',
       currently the 'compares' used reside in pointobj.[ch] */
	qsort((void *)view->S, view->next, sizeof(pointobj *), compare);
}

int load_stars(char *fname, program_state *pstat) {
    /*
    DROP TABLE _cmap;
    SELECT hip, ra, de, vmag, _bv, _hvartype, _multflag, _sptype into _cmap 
           from _hipp where vmag < 6.5 order by vmag, ra, de;
    COPY _cmap TO '/home/rursus/Desktop/dumps/sternons/trunk/star.db'
         DELIMITER '|';
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

pointobj *find_star_by_HIP(int HIP, program_state *pstat) {
    pointobj *s, *res;

    s = new_pointobj(PO_STAR, HIP, 0, 0, 0, 0);
    res = *((pointobj **)bsearch(&s, (void *)pstat->stars[BY_HIP]->S,
                                  pstat->stars[BY_HIP]->next,
                                  sizeof(pointobj *), pointobj_cmp_by_HIP));
    free(s);
    return res;
}

int load_star_lines(char *fname, program_state *pstat) {
    int boldness, HIP1, HIP2;
    double RA1, DE1;
    double RA2, DE2;
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
        HIP1 = next_field_int(&pos);
        S = find_star_by_HIP(HIP1, pstat);
        if (S) {
            RA1 = pointobj_attr_D(S, POA_RA);
            DE1 = pointobj_attr_D(S, POA_DE); /* vmag1 = S->vmag; */
        }
        else {
            RA1 = 666; DE1 = 666; /* vmag1 = 666; */
        }
        HIP2 = next_field_double(&pos);
        S = find_star_by_HIP(HIP2, pstat);
        if (S) {
            RA2 = pointobj_attr_D(S, POA_RA); 
            DE2 = pointobj_attr_D(S, POA_DE); /* vmag2 = S->vmag; */
        }
        else {
            RA2 = 666; DE2 = 666; /* vmag2 = 666; */
        }
        if(0) { char buf[256];
            fprintf(stdout, "line %s %s:\n", 
                    boldness==1?"heavy":(boldness==2?"bold":"light"),
                    ucstombs(buf,asterism,128));
            fprintf(stdout, "  star 1 HIP=%i RA=%f DE=%f\n", HIP1, RA1, DE1);
            fprintf(stdout, "  star 2 HIP=%i RA=%f DE=%f\n", HIP2, RA2, DE2);
        }
        pstat->latest_line = new_line(boldness, asterism, HIP1, HIP2,
                                      pstat->latest_line);
        line_set_pos_1(pstat->latest_line, RA1, DE1);
        line_set_pos_2(pstat->latest_line, RA2, DE2);
    }
    u8fclose(inf);
    return 1;
}

int load_constellation_bounds(char *fname, program_state *pstat) {
    int boldness;
    double RA1, DE1;
    double RA2, DE2;
    uchar line[1024], *pos, *constel;
    utf8_file *inf = u8fopen(fname, "rt");

    if (!inf) return 0;
    while (fgetus(line, 1023, inf)) {
        /* char buf[128]; */
        line[ucslen(line)-1] = L'\0';
        pos = line;
        boldness = ucstoi(pos);
        constel = next_field_ustr(&pos);
        RA1 = next_field_double(&pos)*15;
        DE1 = next_field_double(&pos);
        RA2 = next_field_double(&pos)*15;
        DE2 = next_field_double(&pos);
        if (0) { char buf[256];
            fprintf(stdout, "border %s:\n", ucstombs(buf,constel,128));
            fprintf(stdout, "  start RA=%f DE=%f\n", RA1, DE1);
            fprintf(stdout, "  end RA=%f DE=%f\n", RA2, DE2);
        }
        /* as lines */
        pstat->latest_bound = new_line(4, constel, -1, -1, pstat->latest_bound);
        line_set_pos_1(pstat->latest_bound, RA1, DE1);
        line_set_pos_2(pstat->latest_bound, RA2, DE2);
        /* as polygonal areas */
        pstat->last_const = add_point(constel, RA1, DE1, pstat->last_const);
        if (!pstat->first_const) {
            pstat->first_const = pstat->last_const;
        }
        /** cleanup constel */
    }
    u8fclose(inf);
    /* dump_polygon_set(pstat->first_const); */
    return 1;
}

int load_star_labels(char *fname, program_state *pstat) {
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

#define FONT_STYLE "font-family: FreeSans, sans serif; font-weight: bold;"

void draw_head(program_state *pstat) {
    char buf[256];
    int H, W;
    image_struct *image = pstat->image;
    FILE *out = pstat->out_file;

    W = image->width;
    H = image->height;
    fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\" "
    		     "standalone=\"no\"?>\n");
    fprintf(out, "<svg width=\"%i\" height=\"%i\"\n"
                 "     xmlns=\"http://www.w3.org/2000/svg\"\n"
                 "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
                 "     >\n", W, H);
    fprintf(out, "     <title>%s</title>\n",
    		     ucstombs(buf, pstat->image->name, 256));
}

void draw_background(program_state *pstat) {
    int ix, iy, H, W, H2, W2, dim;
    image_struct *image = pstat->image;
    FILE *out = pstat->out_file;

    W = image->width; W2 = W/2;
    H = image->height; H2 = H/2;
    dim = image->dim;
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
}

void draw_grid(program_state *pstat) {
    int ix, iy, H, W, H2, W2, dim;
    double ra, ras[24], de, des[17];
    double X, Y;
    double x, y;
    image_struct *image = pstat->image;
    proj *projection = image->projection;
    FILE *out = pstat->out_file;

    W = image->width; W2 = W/2;
    H = image->height; H2 = H/2;
    dim = image->dim;
    /* Declination lines: */
    #define NUM_DE 17

    fprintf(out, "    <!-- GRID -->\n");
    for (iy = 0; iy < NUM_DE; iy++) {
        des[iy] = deg2rad(iy*10-80);
    }
    for (ra = 0; ra < 360; ra+=0.5) {
        for (ix = 0; ix < NUM_DE; ix++) {
            project(&X, &Y, des[ix], deg2rad(ra), projection);
            if (pos_in_frame(&x, &y, X, Y, image)) {
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
            if (pos_in_frame(&x, &y, X, Y, image)) {
                fprintf(out, "    <circle cx=\"%.2f\" cy=\"%.2f\" r=\"1\"\n", x, y);
                fprintf(out, "            style=\"opacity:1;fill:#880088;");
                fprintf(out, "fill-opacity:1;\"/>\n");
            }
        }
    }
}

void draw_stars(program_state *pstat) {
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
    image_struct *image = pstat->image;
    proj *projection = image->projection;
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
        if(pos_in_frame(&x, &y, X, Y, image)) {
            size = (6.8-vmag)*0.8*image->scale;
            fprintf(out, "    <circle title=\"HIP %i\" cx=\"%.2f\" cy=\"%.2f\" r=\"%g\"\n",
                             HIP, x, y, size);
            fprintf(out, "            style=\"opacity:1;fill:#FFFFFF;fill-opacity:1;"
                         "stroke:#666666;stroke-width:1px\"/>\n");
        }
    }
    /* dump_stars_by_vmag_view(stderr, pstat->stars_by_vmag); */
}

void draw_line_set(program_state *pstat, uchar *id, line *line_set) {
    line *L;
    double X1, Y1, X2, Y2;
    double x1, y1, x2, y2;
    image_struct *image = pstat->image;
    proj *projection = image->projection;
    FILE *out = pstat->out_file;
    char buf[256];

    for (L = line_set; L; L = L->prev) {
    	if (id && 0 != ucscmp(id,L->asterism)) continue;
        project(&X1, &Y1, deg2rad(L->DE_1), deg2rad(L->RA_1), projection);
        project(&X2, &Y2, deg2rad(L->DE_2), deg2rad(L->RA_2), projection);
        if (pos_in_frame(&x1, &y1, X1, Y1, image)
          | pos_in_frame(&x2, &y2, X2, Y2, image)) {
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
}

void draw_lines(program_state *pstat, uchar *id) {
    draw_line_set(pstat, id, pstat->latest_line);
}

void draw_bounds(program_state *pstat) {
    draw_line_set(pstat, 0, pstat->latest_bound);
}

void draw_delportian_area(program_state *pstat, uchar *id) {
    polygon_set *PS;
    image_struct *image = pstat->image;
    proj *projection = image->projection;
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
            pos_in_frame(&x, &y, X, Y, image);
            fprintf(out, "          d=\"M %.2f,%.2f\n", x, y);
            for (P = P->prev; P; P = P->prev) {
                project(&X, &Y, deg2rad(P->DE), deg2rad(P->RA), projection);
                pos_in_frame(&x, &y, X, Y, image);
                fprintf(out, "             L %.2f,%.2f\n", x, y);
            }
            fprintf(out, "             Z\"\n");
            fprintf(out, "/>\n");
        }
    }
}

void draw_labels(program_state *pstat, uchar *NOTUSEDYET) {
    label *L;
    image_struct *image = pstat->image;
    proj *projection = image->projection;
    FILE *out = pstat->out_file;

    for (L = pstat->latest_star_label; L; L = L->prev) {
        double X, Y;
        double x, y;
        char buf[256], *anchor;
        project(&X, &Y, deg2rad(L->DE), deg2rad(L->RA), projection);
        if (pos_in_frame(&x, &y, X, Y, image)) {
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
}

void draw_debug_info(program_state *pstat) {
    struct tm *TM;
    time_t T = time(NULL);
    int height = pstat->image->height;

    if (pstat->debug == DEBUG) {
        T = time(NULL);
        if (T == -1) return;
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
}

void draw_foot(program_state *pstat) {
    /* printf("    <image y=\"200\" x=\"200\" height=\"100\" width=\"100\"\n"); */
    /* printf("           xlink:href=\"neptune.png\" />\n"); */
    fprintf(pstat->out_file, "</svg>\n");
}

int close_file(program_state *pstat) {
    return fclose(pstat->out_file);
}

void usage_exit(void) {
    /* Usage text here when stabilized */
    exit(-1);
}

void tok_dump(int debug, token *tok) {
    char buf[1024], buf2[1024];

    switch (tok->type) {
      case TOK_STR:
        fprintf(stderr, "⟨%s⟩“%s”", tok_type_str(tok), tok_str(buf, tok, 1023));
        break;
      case TOK_NUM:
        fprintf(stderr, "⟨%s⟩%s(%s)", tok_type_str(tok),
                tok_str(buf, tok, 1023), tok_unit(buf2, tok, 1023));
        break;
      default:                    
        fprintf(stderr, "⟨%s⟩%s", tok_type_str(tok), tok_str(buf, tok, 1023));
    }
}

int main (int argc, char **argv) {
    /* dummy setup: */
    program_state *pstat;
    proj *projection;
    image_struct *image;
    uchar *star_data_tags[] = { u"RA", u"DE", u"V", u"HIP", 0 };

    if (argc != 3) usage_exit();
    /* init: */
    /*TBD: if (!parse_program(argv[1], pstat))
        usage_exit();*/

    /*>Arg handling here! */
    /*>---A₀:   mkmap /stardb/              -- star db only                          ---*/
    /*>---A₁:   mkmap /dummyprog/ /stardb/  -- prog loaded but unused                ---*/
    /*>   A₁,₅: mkmap /dummyprog/ /stardb/  -- prog loaded partially used            ---*/
    /*>   A₂:   mkmap /prog/ /stardb/       -- prog loaded and used for std setting     */
    /*>   A₃:   mkmap /prog/                -- prog also used for star db loading       */
    /*>   A₄:     /dismissed/                                                           */
    /*>   A₅:   mkmap /prog/ /arg₁/ ...     -- make the 2++ arg real arguments         */

	/* Init classes: */
	init_method_tags();
	init_named_class(PO_STAR, star_data_tags);

    pstat = new_program_state(DEBUG, stderr);

    load_stars("star.db", pstat);
    load_star_lines("lines.db", pstat);             /* dependent on load_stars */
    load_constellation_bounds("bounds.db", pstat);  /* dependent on nothing */

    projection = init_Lambert(82.5, 5, 15, 25);
    image = new_image(u"Orion", 500, 500, 1.4);
    program_set_image(pstat, image);
    image_set_projection(image, projection);

    /* generate one output map: */
    if (open_file("orion.svg", pstat)) {
        pstat = program_push(pstat, stderr);
		load_star_labels("orion-labels.db", pstat);
        draw_head(pstat);
        draw_background(pstat);
        draw_bounds(pstat);
        draw_delportian_area(pstat, u"Ori");
        draw_grid(pstat);
        draw_lines(pstat, u"Ori Bdy");
        draw_lines(pstat, u"Ori Arm");
        draw_lines(pstat, u"Ori Shd");
        draw_labels(pstat, u"Ori");
        draw_stars(pstat);
        draw_debug_info(pstat);
        draw_foot(pstat);
        close_file(pstat);
        pstat = program_pop(pstat, stderr);
    }
    else {
        fprintf(stderr, "ERROR: couldn't write file 'orion.svg'\n");
    }

    projection = init_Lambert(106, 0, 10, 20);
    image = new_image(u"Monoceros", 600, 550, 1.4);
    program_set_image(pstat, image);
    image_set_projection(image, projection);

    if (open_file("monoceros.svg", pstat)) {
        pstat = program_push(pstat, stderr);
		load_star_labels("monoceros-labels.db", pstat);
        draw_head(pstat);
        draw_background(pstat);
        draw_bounds(pstat);
        draw_delportian_area(pstat, u"Mon");
        draw_grid(pstat);
        draw_labels(pstat, u"Mon");
        draw_lines(pstat, u"Mon Bdy");
        draw_stars(pstat);
        draw_debug_info(pstat);
        draw_foot(pstat);
        close_file(pstat);
        pstat = program_pop(pstat, stderr);
    }
    else {
        fprintf(stderr, "ERROR: couldn't write file 'monoceros.svg'\n");
    }

    return 0;
}