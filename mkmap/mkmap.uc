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
#include <string.h>
#include <math.h>
#include <time.h>           /* For time stamping the imgs in debug mode */
#include <values.h>

#define NO_DEBUG 0
#define DEBUG 1

/* UTILS */
#include "defs.h"
#include "mathx.h"
#include "allstrings.h"     /* generated strings */
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
#include "token.h"
#include "valstack.h"
#include "progstat.h"

/* BINDING CODE */
#include "ps_db.h"
#include "ps_draw.h"

void usage_exit(void) {
    /* Usage text here when stabilized */
    exit(-1);
}

void tok_dump(int debug, token *tok) {
    char buf[1024], buf2[1024];

    switch (tok->type) {
      case TOK_USTR:
        fprintf(stderr, "⟨%s⟩“%s”", tok_type_str(tok), tok_str(buf, tok, 1023));
        break;
      case TOK_CSTR:
        fprintf(stderr, "⟨%s⟩‘%s’", tok_type_str(tok), tok_str(buf, tok, 1023));
        break;
      case TOK_NUM:
        fprintf(stderr, "⟨%s⟩%s(%s)", tok_type_str(tok),
                tok_str(buf, tok, 1023), tok_unit(buf2, tok, 1023));
        break;
      default:                    
        fprintf(stderr, "⟨%s⟩%s", tok_type_str(tok), tok_str(buf, tok, 1023));
    }
}

int parse_first_map_format(char *fname) {
    return 0;
}

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

#define VM_NULL                   0

#define VM_CSTR                   1 /* up to VM_NULL */
#define VM_USTR                   2 /* up to VM_NULL */
#define VM_INT                    3 /* 1 int */
#define VM_REAL                   4 /* 2 ints */

#define VM_BEGIN                 10
#define VM_END                   11
#define VM_LOAD_STARS            12
#define VM_LOAD_STAR_LINES       13
#define VM_LOAD_CONST_BOUNDS     14
#define VM_NEW_IMAGE             15
#define VM_IMG_LAMBERT           16
#define VM_OPEN_FILE             17
#define VM_LOAD_LABELS           18
#define VM_DRAW_DELPORTIAN_AREA  19
#define VM_DRAW_LINES            20
#define VM_DRAW_LABELS           21
#define VM_DRAW_HEAD             22
#define VM_DRAW_BACKGROUND       23
#define VM_DRAW_BOUNDS           24
#define VM_DRAW_GRID             25
#define VM_DRAW_STARS            26
#define VM_DRAW_DEBUG_INFO       27
#define VM_DRAW_FOOT             28
#define VM_CLOSE_FILE            29

int VM_load_stars(progstat *prog) {
    /*char *opcode_name = "LOADSTARS";*/
    char *fname = PS_pop_cstr(prog);
    load_stars(prog, fname);
    return 1;
}

int VM_load_star_lines(progstat *prog) {
    /*char *opcode_name = "LOADSTARLINES";*/
    char *fname = PS_pop_cstr(prog);
    load_star_lines(prog, fname);
    return 1;
}

int VM_load_const_bounds(progstat *prog) {
    /*char *opcode_name = "LOADCONSTBOUNDS";*/
    char *fname = PS_pop_cstr(prog);
    load_constellation_bounds(prog, fname);
    return 1;
}

int VM_new_image(progstat *prog) {
    /*char *opcode_name = "NEWIMG";*/
    uchar *name; int width, height; double scale;
    scale = PS_pop_dbl(prog);
    height = PS_pop_int(prog);
    width = PS_pop_int(prog);
    name = PS_pop_ustr(prog);
    PS_new_image(prog, name, width, height, scale);
    return 1;
}

int VM_img_Lambert(progstat *prog) {
    /*char *opcode_name = "IMGLAMBERT";*/
    double l0, p0, p1, p2;
    p2 = PS_pop_dbl(prog);
    p1 = PS_pop_dbl(prog);
    p0 = PS_pop_dbl(prog);
    l0 = PS_pop_dbl(prog);
    PS_img_set_Lambert(prog, l0, p0, p1, p2);
    return 1;
}

int VM_open_file(progstat *prog) {
    /*char *opcode_name = "OPENFILE";*/
    char *fname = PS_pop_cstr(prog);
    PS_open_file(fname, prog);
    return 1;
}

int VM_load_labels(progstat *prog) {
    /*char *opcode_name = "LOADLABELS";*/
    char *fname = PS_pop_cstr(prog);
    load_star_labels(prog, fname);
    return 1;
}

int VM_draw_delportian_area(progstat *prog) {
    /*char *opcode_name = "DELPORTE";*/
    uchar *tag_selected = PS_pop_ustr(prog);
    draw_delportian_area(prog, tag_selected);
    return 1;
}

int VM_draw_lines(progstat *prog) {
    /*char *opcode_name = "DRAWLINES";*/
    uchar *tag_selected = PS_pop_ustr(prog);
    draw_lines(prog, tag_selected);
    return 1;
}

int VM_draw_labels(progstat *prog) {
    /*char *opcode_name = "DRAWLABELS";*/
    uchar *tag_selected = PS_pop_ustr(prog);
    draw_labels(prog, tag_selected);
    return 1;
}

int VM_exec(int op, progstat *prog) {
    switch (op) {
        case VM_LOAD_STARS:
            return VM_load_stars(prog);
        case VM_LOAD_STAR_LINES:
            return VM_load_star_lines(prog);
        case VM_LOAD_CONST_BOUNDS:
            return VM_load_const_bounds(prog);
        case VM_NEW_IMAGE:
            return VM_new_image(prog);
        case VM_IMG_LAMBERT:
            return VM_img_Lambert(prog);
        case VM_OPEN_FILE:
            return VM_open_file(prog);
        case VM_LOAD_LABELS:
            return VM_load_labels(prog);
        case VM_DRAW_DELPORTIAN_AREA:
            return VM_draw_delportian_area(prog);
        case VM_DRAW_LINES:
            return VM_draw_lines(prog);
        case VM_DRAW_LABELS:
            return VM_draw_labels(prog);
        case VM_DRAW_HEAD:
            draw_head(prog);
            return 1;
        case VM_DRAW_BACKGROUND:
            draw_background(prog);
            return 1;
        case VM_DRAW_BOUNDS:
            draw_bounds(prog);
            return 1;
        case VM_DRAW_GRID:
            draw_grid(prog);
            return 1;
        case VM_DRAW_STARS:
            draw_stars(prog);
            return 1;
        case VM_DRAW_DEBUG_INFO:
            draw_debug_info(prog);
            return 1;
        case VM_DRAW_FOOT:
            draw_foot(prog);
            return 1;
        case VM_CLOSE_FILE:
            close_file(prog);
            return 1;
        default:
            fprintf(stderr, "ERROR: unknown op code '%i'\n", op);
            return 0;
    }
    return 1;
}

int VM_do(int *op_list, progstat *pstat) {
    int PC = 0;

    for (PC = 0; op_list[PC] != 0; PC++) {
        if (op_list[PC] == VM_CSTR) {
            PC++;
            PS_push_cstr(pstat, (char *)op_list[PC]);
        }
        else if (op_list[PC] == VM_USTR) {
            PC++;
            PS_push_ustr(pstat, (uchar *)op_list[PC]);
        }
        else {
            VM_exec(op_list[PC], pstat);
        }
    }
    return 1;
}

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

int main (int argc, char **argv) {
    /* dummy setup: */
    progstat *pstat;
    uchar *star_data_tags[] = { u"RA", u"DE", u"V", u"HIP", 0 };

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

    if (argc >= 2) {
        int aix;

        init_method_tags();
        init_named_class(PO_STAR, star_data_tags);
        pstat = new_progstat(DEBUG, stderr);

        for (aix = 1; aix < argc; aix++) {
            char *ext = strrchr(argv[aix],'.');

            if(0 == strcmp(ext, ".ma1")) {
                /* FIRST MAP FORMAT */
                
            }
            else {
                fprintf(stderr, "WARNING: ignoring file '%s' in unrecognized"
                                " map format '%s'\n", argv[aix], ext);
            }
        }
    }

    if (1 && argc == 1) {
        /** PROG MUST BE LOADED:                  */
        /** 1. load it raw                        */
        /** 2. resolve constants, address them(?) */

        /* IF NO ARGUMENTS ARE GIVEN, ORION AND MONOCEROS ARE GENERATED */

        /* Init classes: */
        init_method_tags();
        init_named_class(PO_STAR, star_data_tags);
        pstat = new_progstat(DEBUG, stderr);

        /** ALL-SKY DATABASES LOAD **/
        /*load_stars(pstat, "star.db");*/
        PS_push_cstr(pstat, "star.db");
        VM_exec(VM_LOAD_STARS, pstat);
        /*load_star_lines(pstat, "lines.db"); */ /* dependent on load_stars */
        PS_push_cstr(pstat, "lines.db");
        VM_exec(VM_LOAD_STAR_LINES, pstat);
        /*load_constellation_bounds(pstat, "bounds.db");*/ /* dependent on nothing */
        PS_push_cstr(pstat, "bounds.db");
        VM_exec(VM_LOAD_CONST_BOUNDS, pstat);

        { /* image orion */
            /*PS_new_image(pstat, u"Orion", 500, 500, 1.4);*/
            PS_push_ustr(pstat, u"Orion");
            PS_push_int(pstat, 500);
            PS_push_int(pstat, 500);
            PS_push_dbl(pstat, 1.4);
            VM_exec(VM_NEW_IMAGE, pstat);
            /*PS_img_set_Lambert(pstat, 82.5, 5, 15, 25);*/
            PS_push_dbl(pstat, 82.5);
            PS_push_dbl(pstat, 5);
            PS_push_dbl(pstat, 15);
            PS_push_dbl(pstat, 25);
            VM_exec(VM_IMG_LAMBERT, pstat);

            /* generate one output map: */
            PS_push_cstr(pstat, "orion.svg");
            if (VM_open_file(pstat)) {
                int image_setup_code[] = {
                    VM_CSTR, (int)"orion-labels.db", VM_LOAD_LABELS, 0
                };
                int init_drawing_code[] = {
                    VM_DRAW_HEAD, VM_DRAW_BACKGROUND, 0
                };
                int support_drawing_code[] = {
                    VM_DRAW_BOUNDS, 
                    VM_USTR, (int)u"Ori", VM_DRAW_DELPORTIAN_AREA,
                    VM_DRAW_GRID,
                    VM_USTR, (int)u"Ori Bdy", VM_DRAW_LINES,
                    VM_USTR, (int)u"Ori Arm", VM_DRAW_LINES,
                    VM_USTR, (int)u"Ori Shd", VM_DRAW_LINES,
                    VM_USTR, (int)u"Ori", VM_DRAW_LABELS,
                    0
                };
                int real_objects_code[] = {
                    VM_DRAW_STARS, 0
                };
                int final_code[] = {
                    VM_DRAW_DEBUG_INFO, VM_DRAW_FOOT, VM_CLOSE_FILE, 0
                };

                pstat = PS_push(pstat, stderr);
                VM_do(image_setup_code, pstat);
                VM_do(init_drawing_code, pstat);
                VM_do(support_drawing_code, pstat);
                VM_do(real_objects_code, pstat);
                VM_do(final_code, pstat);
                pstat = PS_pop(pstat, stderr);
            }
            else {
                fprintf(stderr, "ERROR: couldn't write file 'orion.svg'\n");
            }
        }

        {
            /*PS_new_image(pstat, u"Monoceros", 600, 550, 1.4);*/
            PS_push_ustr(pstat, u"Monoceros");
            PS_push_int(pstat, 600);
            PS_push_int(pstat, 550);
            PS_push_dbl(pstat, 1.4);
            VM_new_image(pstat);
            /*PS_img_set_Lambert(pstat, 106, 0, 10, 20);*/
            PS_push_dbl(pstat, 106);
            PS_push_dbl(pstat, 0);
            PS_push_dbl(pstat, 10);
            PS_push_dbl(pstat, 20);
            VM_img_Lambert(pstat);

            PS_push_cstr(pstat, "monoceros.svg");
            if (VM_open_file(pstat)) {
                int image_setup_code[] = {
                    VM_CSTR, (int)"monoceros-labels.db", VM_LOAD_LABELS, 0
                };
                int init_drawing_code[] = {
                    VM_DRAW_HEAD, VM_DRAW_BACKGROUND, 0
                };
                int support_drawing_code[] = {
                    VM_DRAW_BOUNDS, 
                    VM_USTR, (int)u"Mon", VM_DRAW_DELPORTIAN_AREA,
                    VM_DRAW_GRID,
                    VM_USTR, (int)u"Mon Bdy", VM_DRAW_LINES,
                    VM_USTR, (int)u"Mon", VM_DRAW_LABELS,
                    0
                };
                int real_objects_code[] = {
                    VM_DRAW_STARS, 0
                };
                int final_code[] = {
                    VM_DRAW_DEBUG_INFO, VM_DRAW_FOOT, VM_CLOSE_FILE, 0
                };

                pstat = PS_push(pstat, stderr);
                VM_do(image_setup_code, pstat);
                VM_do(init_drawing_code, pstat);
                VM_do(support_drawing_code, pstat);
                VM_do(real_objects_code, pstat);
                VM_do(final_code, pstat);
                pstat = PS_pop(pstat, stderr);
            }
            else {
                fprintf(stderr, "ERROR: couldn't write file 'monoceros.svg'\n");
            }
        }
    }

    return 0;
}

/* :mode=c: */
