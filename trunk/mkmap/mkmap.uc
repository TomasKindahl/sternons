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

/* STANDARD LIBS */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>           /* For time stamping the imgs in debug mode */

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
#include "progstat.h"
#include "vmcode.h"

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

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

void get_signature(FILE *inf, int num, char *store) {
    int ix, ch;
    for(ch = fgetc(inf), ix = 0; ix < num; ch = fgetc(inf), ix++) {
        store[ix] = ch;
    }
    store[ix] = '\0';
}

int ishex(int ch) {
    return ('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F')
        || ('a' <= ch && ch <= 'f');
}

int isalusc(int ch) {
    return (ch == '_') || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z');
}

int gethex(int ch) {
    if ('0' <= ch && ch <= '9') return ch - '0';
    if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
    if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
    return -1;
}

int isws(int ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
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
        PS_push_cstr(pstat, "db/star.db");
        VM_exec(VM_LOAD_STARS, pstat, -1);
        /*load_star_lines(pstat, "lines.db"); */ /* dependent on load_stars */
        PS_push_cstr(pstat, "db/lines.db");
        VM_exec(VM_LOAD_STAR_LINES, pstat, -1);
        /*load_constellation_bounds(pstat, "bounds.db");*/ /* dependent on nothing */
        PS_push_cstr(pstat, "db/bounds.db");
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
        PS_push_cstr(pstat, "db/star.db");
        VM_exec(VM_LOAD_STARS, pstat, -1);
        /*load_star_lines(pstat, "lines.db"); */ /* dependent on load_stars */
        PS_push_cstr(pstat, "db/lines.db");
        VM_exec(VM_LOAD_STAR_LINES, pstat, -1);
        /*load_constellation_bounds(pstat, "bounds.db");*/ /* dependent on nothing */
        PS_push_cstr(pstat, "db/bounds.db");
        VM_exec(VM_LOAD_CONST_BOUNDS, pstat, -1);

        { /* image orion */
            uchar *ustr[] = {
                u"Orion", u"Ori", u"Ori Bdy", u"Ori Arm",
                u"Ori Shd", 0
            };
            char *cstr[] = {
                "db/orion-labels.db", "images/orion-default.svg", 0
            };
            optype set_settings_code[] = {
                26,
                VM_NEW_IMAGE,
                /**name “Orion”;*/
                VM_USTR, (optype)0, VM_IMG_SET_NAME,
                /**size 500 500;*/
                VM_INT, 500, VM_INT, 500, VM_IMG_SET_SIZE,
                /**scale 1.4;*/
                VM_DBL, 0x3ff66666, 0x66666666, VM_IMG_SET_SCALE,
                /**projection Lambert 82.5 5 15 25;*/
                VM_DBL, 0x4054a000, 0x00000000, /* = 82.5 */
                VM_DBL, 0x40140000, 0x00000000, /* = 5    */
                VM_DBL, 0x402e0000, 0x00000000, /* = 15   */
                VM_DBL, 0x40390000, 0x00000000, /* = 25   */
                VM_IMG_SET_LAMBERT
            };
            /** image_data: */
            optype image_setup_code[] = {
                3,
                /** load labels ‘orion-labels.db’; */
                VM_CSTR, (optype)0, VM_LOAD_LABELS
            };
            /** init_drawing: */
            optype init_drawing_code[] = {
                5, 
                /** open file ‘orion.svg’; */
                VM_CSTR, (optype)1, VM_OPEN_FILE,
                /** draw head; draw background; */
                VM_DRAW_HEAD, VM_DRAW_BACKGROUND
            };
            /** support_drawing: */
            optype support_drawing_code[] = {
                17,
                /** draw bounds; */
                VM_DRAW_BOUNDS,
                /** draw delportian select “Ori”; */       
                VM_USTR, (optype)1, VM_DRAW_DELPORTIAN_AREA,
                /** draw grid; */
                VM_DRAW_GRID,
                /** draw lines select “Ori Bdy”; */
                VM_USTR, (optype)2, VM_DRAW_LINES,
                /** draw lines select “Ori Arm”; */
                VM_USTR, (optype)3, VM_DRAW_LINES,
                /** draw lines select “Ori Shd”; */
                VM_USTR, (optype)4, VM_DRAW_LINES,
                /** draw labels select “Ori”; */
                VM_USTR, (optype)1, VM_DRAW_LABELS
            };
            /** real_objects: */
            optype real_objects_code[] = {
                1,
                /** draw stars; */
                VM_DRAW_STARS
            };
            /** final: */
            optype final_code[] = {
                3,
                /** draw debug info; draw footer; close file; */
                VM_DRAW_DEBUG_INFO, VM_DRAW_FOOT, VM_CLOSE_FILE
            };
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

            /* FOR DEBUG: VM_dump_image_program(&orion_program); */
            /* generate one output map: */
            VM_draw_image(&orion_program, pstat);
            /* REPLACE WITH:
              PS_assign_var(pstat, "orion", orion_program);
              PS_recall_var(pstat, "orion");
              PS_push_cstr(pstat, "orion.svg");
              VM_draw_image_stack(pstat);
            */

            fprintf(stderr, "DONE\n");
        }

        {
            uchar *ustr[] = {
                u"Monoceros", u"Mon", u"Mon Bdy", 0
            };
            char *cstr[] = {
                "db/monoceros-labels.db", "images/monoceros-default.svg", 0
            };
            optype set_settings_code[] = {
                26,
                VM_NEW_IMAGE,
                /**set name = "Monoceros";*/
                VM_USTR, (optype)0, VM_IMG_SET_NAME,
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
                VM_CSTR, (optype)0, VM_LOAD_LABELS
            };
            optype init_drawing_code[] = {
                5,
                VM_CSTR, (optype)1, VM_OPEN_FILE,
                VM_DRAW_HEAD, VM_DRAW_BACKGROUND
            };
            optype support_drawing_code[] = {
                11,
                VM_DRAW_BOUNDS,
                VM_USTR, (optype)1, VM_DRAW_DELPORTIAN_AREA,
                VM_DRAW_GRID,
                VM_USTR, (optype)2, VM_DRAW_LINES,
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
            image_program monoceros_program; {
                int ix;
                VM_init_image_program(&monoceros_program);
                for (ix = 0; ustr[ix]; ix++) {
                    VM_add_ustring(&monoceros_program, ustr[ix]);
                }
                for (ix = 0; cstr[ix]; ix++) {
                    VM_add_cstring(&monoceros_program, cstr[ix]);
                }
                VM_add_code_layer(&monoceros_program, set_settings_code);
                VM_add_code_layer(&monoceros_program, image_setup_code);
                VM_add_code_layer(&monoceros_program, init_drawing_code);
                VM_add_code_layer(&monoceros_program, support_drawing_code);
                VM_add_code_layer(&monoceros_program, real_objects_code);
                VM_add_code_layer(&monoceros_program, final_code);
            }

            fprintf(stderr, "Monoceros ... "); fflush(stderr);

            VM_draw_image(&monoceros_program, pstat);
            /* REPLACE WITH:
              PS_assign_var(pstat, "monoceros", monoceros_program);
              PS_recall_var(pstat, "monoceros");
              PS_push_cstr(pstat, "monoceros.svg");
              VM_draw_image_stack(pstat);
            */

            fprintf(stderr, "DONE\n");
        }
    }

    return 0;
}

/* :mode=C: */
