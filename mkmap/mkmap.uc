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
#include "vmcode.h"
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
    PS_new_image(prog);
    return 1;
}

int VM_img_set_name(progstat *prog) {
    uchar *uname = PS_pop_ustr(prog);
    PS_img_set_name(prog, uname);
    return 1;
}

int VM_img_set_size(progstat *prog) {
    int width, height;
    height = PS_pop_int(prog);
    width  = PS_pop_int(prog);
    PS_img_set_size(prog, width, height);
    return 1;
}

int VM_img_set_scale(progstat *prog) {
    double scale = PS_pop_dbl(prog);
    PS_img_set_scale(prog, scale);
    return 1;
}

int VM_img_set_Lambert(progstat *prog) {
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

int VM_exec(int op, progstat *prog, int code_num) {
    switch (op) {
        case VM_LOAD_STARS:
            return VM_load_stars(prog);
        case VM_LOAD_STAR_LINES:
            return VM_load_star_lines(prog);
        case VM_LOAD_CONST_BOUNDS:
            return VM_load_const_bounds(prog);
        case VM_NEW_IMAGE:
            return VM_new_image(prog);
        case VM_IMG_SET_NAME:
            return VM_img_set_name(prog);
        case VM_IMG_SET_SIZE:
            return VM_img_set_size(prog);
        case VM_IMG_SET_SCALE:
            return VM_img_set_scale(prog);
        case VM_IMG_SET_LAMBERT:
            return VM_img_set_Lambert(prog);
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
            return draw_head(prog);
        case VM_DRAW_BACKGROUND:
            return draw_background(prog);
        case VM_DRAW_BOUNDS:
            return draw_bounds(prog);
        case VM_DRAW_GRID:
            return draw_grid(prog);
        case VM_DRAW_STARS:
            return draw_stars(prog);
        case VM_DRAW_DEBUG_INFO:
            return draw_debug_info(prog);
        case VM_DRAW_FOOT:
            return draw_foot(prog);
        case VM_CLOSE_FILE:
            return close_file(prog);
        default:
            fprintf(stderr, "VM_exec ERROR: unknown op code '%i' at "
                    "position %i\n", op, code_num);
            return 0;
    }
    return 1;
}

double to_double(int L0, int L1) {
    union {
        double D;
        int L[2];
    } res;
    res.L[0] = L1; res.L[1] = L0;
    return res.D;
}

int VM_do(optype *op_list, progstat *pstat) {
    int PC = 0, PCend;

    if(!op_list) return 0;

    for (PC = 1, PCend = op_list[0]; PC <= PCend; PC++) {
        if (op_list[PC] == VM_CSTR) {
            PC++;
            PS_push_cstr(pstat, (char *)op_list[PC]);
        }
        else if (op_list[PC] == VM_USTR) {
            PC++;
            PS_push_ustr(pstat, (uchar *)op_list[PC]);
        }
        else if (op_list[PC] == VM_INT) {
            PC++;
            PS_push_int(pstat, op_list[PC]);
        }
        else if (op_list[PC] == VM_DBL) {
            double R;
            /*printf("op_list[%i] = %i\n", PC, op_list[PC]); fflush(stdout);*/
            PC++;
            R = to_double(op_list[PC], op_list[PC+1]);
            /*printf("R = %f (%x %x)\n", R, op_list[PC], op_list[PC+1]); fflush(stdout);*/
            PC++;
            PS_push_dbl(pstat, R);
        }
        else {
            VM_exec(op_list[PC], pstat, PC);
        }
    }
    return 1;
}

int VM_draw_image(image_program *iprog, progstat *pstat) {
    int ix;
    char *fname = PS_get_cstr(pstat);
    if (VM_open_file(pstat)) {
        pstat = PS_push(pstat, stderr);
        for (ix = 0; iprog->layer[ix]; ix++)
            VM_do(iprog->layer[ix], pstat);
        pstat = PS_pop(pstat, stderr);
    }
    else {
        fprintf(stderr, "ERROR: couldn't write file '%s'\n", fname);
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
            else if(0 == strcmp(ext, ".mbf")) {
                /* MAP BYTE FORMAT */
                
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
        VM_exec(VM_LOAD_STARS, pstat, -1);
        /*load_star_lines(pstat, "lines.db"); */ /* dependent on load_stars */
        PS_push_cstr(pstat, "lines.db");
        VM_exec(VM_LOAD_STAR_LINES, pstat, -1);
        /*load_constellation_bounds(pstat, "bounds.db");*/ /* dependent on nothing */
        PS_push_cstr(pstat, "bounds.db");
        VM_exec(VM_LOAD_CONST_BOUNDS, pstat, -1);

        { /* image orion */
            optype set_settings_code[] = {
                26,
                VM_NEW_IMAGE,
                /**set name = "Orion";*/
                VM_USTR, (optype)u"Orion", VM_IMG_SET_NAME,
                /**set size = [500, 500];*/
                VM_INT, 500, VM_INT, 500, VM_IMG_SET_SIZE,
                /**set scale = 1.4;*/
                VM_DBL, 0x3ff66666, 0x66666666, VM_IMG_SET_SCALE,
                /**set projection = [Lambert, 82.5, 5, 15, 25];*/
                VM_DBL, 0x4054a000, 0x00000000, /* = 82.5 */
                VM_DBL, 0x40140000, 0x00000000, /* = 5    */
                VM_DBL, 0x402e0000, 0x00000000, /* = 15   */
                VM_DBL, 0x40390000, 0x00000000, /* = 25   */
                VM_IMG_SET_LAMBERT
            };
            optype image_setup_code[] = {
                3, 
                VM_CSTR, (optype)"orion-labels.db", VM_LOAD_LABELS
            };
            optype init_drawing_code[] = {
                2, 
                VM_DRAW_HEAD, VM_DRAW_BACKGROUND
            };
            optype support_drawing_code[] = {
                17,
                VM_DRAW_BOUNDS,
                VM_USTR, (optype)u"Ori", VM_DRAW_DELPORTIAN_AREA,
                VM_DRAW_GRID,
                VM_USTR, (optype)u"Ori Bdy", VM_DRAW_LINES,
                VM_USTR, (optype)u"Ori Arm", VM_DRAW_LINES,
                VM_USTR, (optype)u"Ori Shd", VM_DRAW_LINES,
                VM_USTR, (optype)u"Ori", VM_DRAW_LABELS
            };
            optype real_objects_code[] = {
                1,
                VM_DRAW_STARS
            };
            optype final_code[] = {
                3,
                VM_DRAW_DEBUG_INFO, VM_DRAW_FOOT, VM_CLOSE_FILE
            };
            int ix;
            image_program orion_program; {
                VM_init_image_program(&orion_program);
                VM_add_code_layer(&orion_program, set_settings_code);
                VM_add_code_layer(&orion_program, image_setup_code);
                VM_add_code_layer(&orion_program, init_drawing_code);
                VM_add_code_layer(&orion_program, support_drawing_code);
                VM_add_code_layer(&orion_program, real_objects_code);
                VM_add_code_layer(&orion_program, final_code);
            }

            fprintf(stderr, "Orion ... "); fflush(stderr);

            /* generate one output map: */
            PS_push_cstr(pstat, "orion.svg");
            VM_draw_image(&orion_program, pstat);

            fprintf(stderr, "DONE\n");
        }

        {
            optype set_settings_code[] = {
                26,
                VM_NEW_IMAGE,
                /**set name = "Monoceros";*/
                VM_USTR, (optype)u"Monoceros", VM_IMG_SET_NAME,
                /**set size = [600, 550];*/
                VM_INT, 600, VM_INT, 550, VM_IMG_SET_SIZE,
                /**set scale = 1.4;*/
                VM_DBL, 0x3ff66666, 0x66666666, VM_IMG_SET_SCALE,
                /**set projection = [Lambert, 106, 0, 10, 20];*/
                VM_DBL, 0x405a8000, 0x00000000, /* = 106  */
                VM_DBL, 0x00000000, 0x00000000, /* = 0    */
                VM_DBL, 0x40240000, 0x00000000, /* = 10   */
                VM_DBL, 0x40340000, 0x00000000, /* = 20   */
                VM_IMG_SET_LAMBERT
            };
            optype image_setup_code[] = {
                3,
                VM_CSTR, (optype)"monoceros-labels.db", VM_LOAD_LABELS
            };
            optype init_drawing_code[] = {
                2,
                VM_DRAW_HEAD, VM_DRAW_BACKGROUND
            };
            optype support_drawing_code[] = {
                11,
                VM_DRAW_BOUNDS,
                VM_USTR, (optype)u"Mon", VM_DRAW_DELPORTIAN_AREA,
                VM_DRAW_GRID,
                VM_USTR, (optype)u"Mon Bdy", VM_DRAW_LINES,
                VM_USTR, (optype)u"Mon", VM_DRAW_LABELS
            };
            optype real_objects_code[] = {
                1,
                VM_DRAW_STARS
            };
            optype final_code[] = {
                3,
                VM_DRAW_DEBUG_INFO, VM_DRAW_FOOT, VM_CLOSE_FILE
            };
            int ix;
            image_program monoceros_program; {
                VM_init_image_program(&monoceros_program);
                VM_add_code_layer(&monoceros_program, set_settings_code);
                VM_add_code_layer(&monoceros_program, image_setup_code);
                VM_add_code_layer(&monoceros_program, init_drawing_code);
                VM_add_code_layer(&monoceros_program, support_drawing_code);
                VM_add_code_layer(&monoceros_program, real_objects_code);
                VM_add_code_layer(&monoceros_program, final_code);
            }

            fprintf(stderr, "Monoceros ... "); fflush(stderr);

            PS_push_cstr(pstat, "monoceros.svg");
            VM_draw_image(&monoceros_program, pstat);

            fprintf(stderr, "DONE\n");
        }
    }

    return 0;
}

/* :mode=C: */
