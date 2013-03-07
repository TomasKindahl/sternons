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

int VM_do(image_program *IP, int curr, progstat *pstat) {
    int PC = 0, PCend;
    optype *op_list = IP->layer.arr[curr];

    if(!op_list) return 0;

    for (PC = 1, PCend = op_list[0]; PC <= PCend; PC++) {
        int opval;
        if (op_list[PC] == VM_CSTR) {
            PC++;
            opval = (int)op_list[PC];
            if(opval < 32)
                PS_push_cstr(pstat, IP->cstr.arr[opval]);
            else
                PS_push_cstr(pstat, (char *)op_list[PC]);
        }
        else if (op_list[PC] == VM_USTR) {
            PC++;
            opval = (int)op_list[PC];
            if(opval < 32)
                PS_push_ustr(pstat, IP->ustr.arr[opval]);
            else
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
        for (ix = 0; iprog->layer.arr[ix]; ix++)
            VM_do(iprog, ix, pstat);
        pstat = PS_pop(pstat, stderr);
    }
    else {
        fprintf(stderr, "ERROR: couldn't write file '%s'\n", fname);
    }
    return 1;
}

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

int ishex(char ch) {
    return ('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F')
        || ('a' <= ch && ch <= 'f');
}

int isalusc(char ch) {
    return (ch == '_') || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
}

int gethex(char ch) {
    if ('0' <= ch && ch <= '9') return ch - '0';
    if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
    if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
    return -1;
}

int isws(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
}

void get_signature(FILE *inf, int num, char *store) {
    int ix, ch;
    for(ch = fgetc(inf), ix = 0; ix < num; ch = fgetc(inf), ix++) {
        store[ix] = ch;
    }
    store[ix] = '\0';
}

int translate_label_to_VM(char *cstr) {
    /* printf("translate_label_to_VM: '%s'\n", cstr); */
    /** CODE LABELS: **/
    if(0 == strcmp(cstr,"CSTR")) return VM_CSTR_STORE;
    if(0 == strcmp(cstr,"USTR")) return VM_USTR_STORE;
    if(0 == strcmp(cstr,"SETTINGS")) return VM_SETTINGS_LAYER;
    if(0 == strcmp(cstr,"IMAGE_DATA")) return VM_IMAGE_DATA_LAYER;
    if(0 == strcmp(cstr,"INIT_DRAWING")) return VM_INIT_DRAWING_LAYER;
    if(0 == strcmp(cstr,"SUPPORT_DRAWING")) return VM_SUPPORT_DRAWING_LAYER;
    if(0 == strcmp(cstr,"REAL_OBJECTS")) return VM_REAL_OBJECTS_LAYER;
    if(0 == strcmp(cstr,"FINAL")) return VM_FINAL_LAYER;
    /** CONSTANT LOADER OPERATORS: **/
    if(0 == strcmp(cstr,"cstr")) return VM_CSTR;
    if(0 == strcmp(cstr,"ustr")) return VM_USTR;
    if(0 == strcmp(cstr,"int")) return VM_INT;
    if(0 == strcmp(cstr,"dbl")) return VM_DBL;
    /** OPERATORS: **/
    if(0 == strcmp(cstr,"NewImage")) return VM_NEW_IMAGE;
    if(0 == strcmp(cstr,"ImgSetName")) return VM_IMG_SET_NAME;
    if(0 == strcmp(cstr,"ImgSetSize")) return VM_IMG_SET_SIZE;
    if(0 == strcmp(cstr,"ImgSetScale")) return VM_IMG_SET_SCALE;
    if(0 == strcmp(cstr,"ImgSetLambert")) return VM_IMG_SET_LAMBERT;
    if(0 == strcmp(cstr,"LoadLabels")) return VM_LOAD_LABELS;
    if(0 == strcmp(cstr,"DrawDelportianArea")) return VM_DRAW_DELPORTIAN_AREA;
    if(0 == strcmp(cstr,"DrawLines")) return VM_DRAW_LINES;
    if(0 == strcmp(cstr,"DrawLabels")) return VM_DRAW_LABELS;
    if(0 == strcmp(cstr,"DrawHead")) return VM_DRAW_HEAD;
    if(0 == strcmp(cstr,"DrawBackground")) return VM_DRAW_BACKGROUND;
    if(0 == strcmp(cstr,"DrawBounds")) return VM_DRAW_BOUNDS;
    if(0 == strcmp(cstr,"DrawGrid")) return VM_DRAW_GRID;
    if(0 == strcmp(cstr,"DrawStars")) return VM_DRAW_STARS;
    if(0 == strcmp(cstr,"DrawDebugInfo")) return VM_DRAW_DEBUG_INFO;
    if(0 == strcmp(cstr,"DrawFoot")) return VM_DRAW_FOOT;
    if(0 == strcmp(cstr,"CloseFile")) return VM_CLOSE_FILE;
    return -1;
}

int read_mkmap_BF1(FILE *inf, image_program *iprog) {
    int ch, nx, num, vec[128], lbl_nx;
    char lbl[32];
    enum { NONUM, NUM, LABEL, DELIM } scanstat = NONUM;

    nx = 0;
    for(ch = fgetc(inf); !feof(inf); ch = fgetc(inf)) {
        /* printf("%c", ch); */
        switch(scanstat) {
            case NONUM:
                if(ishex(ch)) {
                    scanstat = NUM;
                    num = gethex(ch);
                }
                else if (ch == ':') {
                    scanstat = LABEL;
                    lbl_nx = 0;
                }
                else if (ch == '-') {
                    int ix;

                    switch(vec[0]) {
                        case VM_CSTR_STORE:
                            VM_add_cstring_from_ustring(iprog, nx-1, &vec[1]);
                            break;
                        case VM_USTR_STORE:
                            VM_add_ustring_length(iprog, nx-1, &vec[1]);
                            break;
                        case VM_SETTINGS_LAYER:
                        case VM_IMAGE_DATA_LAYER:
                        case VM_INIT_DRAWING_LAYER:
                        case VM_SUPPORT_DRAWING_LAYER:
                        case VM_REAL_OBJECTS_LAYER:
                        case VM_FINAL_LAYER:
                            VM_add_code_layer_length(iprog, nx-1, &vec[1]);
                            break;
                    }
                    printf("%X", vec[0]); fflush(stdout);
                    if(vec[0] >= 0x3)
                        printf(" %X", nx-1); fflush(stdout);
                    for (ix = 1; ix < nx; ix++) {
                        printf(" %X", vec[ix]); fflush(stdout);
                    }
                    printf("\n"); fflush(stdout);
                    nx = 0;
                }
                break;
            case NUM:
                if(isws(ch)) {
                    scanstat = NONUM;
                    vec[nx++] = num;
                }
                else if(ishex(ch)) {
                    num = (num<<4) | gethex(ch);
                }
                break;
            case LABEL:
                if(isws(ch)) {
                    scanstat = NONUM;
                    lbl[lbl_nx] = '\0';
                    vec[nx++] = translate_label_to_VM(lbl);
                }
                else if(isalusc(ch)) {
                    lbl[lbl_nx++] = ch;
                }
                break;
            case DELIM: 
                break;
        }
    }
    return 1;
}

int read_mkmap_bin_file(char *file_name, image_program *iprog) {
    char sig[6];
    FILE *inf;

    /* file opening stuff: */
    if(!(inf = fopen(file_name, "rt"))) {
        fprintf(stderr, "FATAL: couldn't open file %s\n", file_name);
        return 0;
    }

    /* read signature, expected to be fixed size 5 */
    get_signature(inf, 4, sig);

    /* for now rejecting anything but mkmap binary 1 format */
    if(0 != strcmp(sig, "mbf1")) {
        fprintf(stderr, "FATAL: not a recognized file format %s\n", sig);
        return 0;
    }

    /* assuming mkmB1, mkmap binary 1, read the stuff */
    return read_mkmap_BF1(inf, iprog);
}

int main(int argc, char **argv) {
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

        for (aix = 1; aix < argc; aix++) {
            char *ext = strrchr(argv[aix],'.');

            if(0 == strcmp(ext, ".mkm")) {
                /* MAP PROGRAM FORMAT (AUTODETECT BY SIGNATURE) */
                printf("MKM\n");
            }
            else if(0 == strcmp(ext, ".mbf")) {
                /* MAP BYTE FORMAT (AUTODETECT BY SIGNATURE) */
                image_program mbf_code;
                printf("MBF\n");
                VM_init_image_program(&mbf_code);
                read_mkmap_bin_file(argv[aix], &mbf_code);
                VM_dump_image_program(&mbf_code);
                PS_push_cstr(pstat, "orion2.svg");
                VM_draw_image(&mbf_code, pstat);
            }
            else {
                fprintf(stderr, "WARNING: ignoring file '%s' in unrecognized"
                                " map format '%s'\n", argv[aix], ext);
            }
        }
    }

    if (argc == 1) {
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
            uchar *ustr[] = {
                u"Orion", u"Ori", u"Ori Bdy", u"Ori Arm",
                u"Ori Shd", 0
            };
            char *cstr[] = {
                "orion-labels.db", 0
            };
            optype set_settings_code[] = {
                26,
                VM_NEW_IMAGE,
                /**set name = "Orion";*/
                VM_USTR, (optype)0, VM_IMG_SET_NAME,
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
                VM_CSTR, (optype)0, VM_LOAD_LABELS
            };
            optype init_drawing_code[] = {
                2, 
                VM_DRAW_HEAD, VM_DRAW_BACKGROUND
            };
            optype support_drawing_code[] = {
                17,
                VM_DRAW_BOUNDS,
                VM_USTR, (optype)1, VM_DRAW_DELPORTIAN_AREA,
                VM_DRAW_GRID,
                VM_USTR, (optype)2, VM_DRAW_LINES,
                VM_USTR, (optype)3, VM_DRAW_LINES,
                VM_USTR, (optype)4, VM_DRAW_LINES,
                VM_USTR, (optype)1, VM_DRAW_LABELS
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
                int ix;
                VM_init_image_program(&orion_program);
                for (ix = 0; ustr[ix]; ix++) {
                    VM_add_ustring(&orion_program, ustr[ix]);
                }
                for (ix = 0; cstr[ix]; ix++) {
                    VM_add_cstring(&orion_program, cstr[ix]);
                }
                VM_add_code_layer(&orion_program, set_settings_code);
                VM_add_code_layer(&orion_program, image_setup_code);
                VM_add_code_layer(&orion_program, init_drawing_code);
                VM_add_code_layer(&orion_program, support_drawing_code);
                VM_add_code_layer(&orion_program, real_objects_code);
                VM_add_code_layer(&orion_program, final_code);
            }

            fprintf(stderr, "Orion ... "); fflush(stderr);

            VM_dump_image_program(&orion_program);
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
