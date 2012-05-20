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

int VM_new_image(progstat *prog) {
    char *opcode_name = "NEWIMG";
    uchar *name; int width, height; double scale;
    scale = PS_pop_dbl(prog);
    height = PS_pop_int(prog);
    width = PS_pop_int(prog);
    name = PS_pop_ustr(prog);
    PS_new_image(prog, name, width, height, scale);
    return 1;
}

int VM_img_Lambert(progstat *prog) {
    char *opcode_name = "IMGLAMBERT";
    double l0, p0, p1, p2;
    p2 = PS_pop_dbl(prog);
    p1 = PS_pop_dbl(prog);
    p0 = PS_pop_dbl(prog);
    l0 = PS_pop_dbl(prog);
    PS_img_set_Lambert(prog, l0, p0, p1, p2);
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
                printf("WARNING: ignoring file '%s' in unrecognized map "
                       "format '%s'\n", argv[aix], ext);
            }
        }
    }

    if (1 && argc == 1) {
        /* IF NO ARGUMENTS ARE GIVEN, ORION AND MONOCEROS ARE GENERATED */

        /* Init classes: */
        init_method_tags();
        init_named_class(PO_STAR, star_data_tags);
        pstat = new_progstat(DEBUG, stderr);

        load_stars(pstat, "star.db");
        load_star_lines(pstat, "lines.db");             /* dependent on load_stars */
        load_constellation_bounds(pstat, "bounds.db");  /* dependent on nothing */

        /*PS_new_image(pstat, u"Orion", 500, 500, 1.4);*/
        PS_push_ustr(pstat, u"Orion");
        PS_push_int(pstat, 500);
        PS_push_int(pstat, 500);
        PS_push_dbl(pstat, 1.4);
        VM_new_image(pstat);
        /*PS_img_set_Lambert(pstat, 82.5, 5, 15, 25);*/
        PS_push_dbl(pstat, 82.5);
        PS_push_dbl(pstat, 5);
        PS_push_dbl(pstat, 15);
        PS_push_dbl(pstat, 25);
        VM_img_Lambert(pstat);

        /* generate one output map: */
        if (PS_open_file("orion.svg", pstat)) {
            pstat = PS_push(pstat, stderr);
            load_star_labels(pstat, "orion-labels.db");
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
            pstat = PS_pop(pstat, stderr);
        }
        else {
            fprintf(stderr, "ERROR: couldn't write file 'orion.svg'\n");
        }

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

        if (PS_open_file("monoceros.svg", pstat)) {
            pstat = PS_push(pstat, stderr);
            load_star_labels(pstat, "monoceros-labels.db");
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
            pstat = PS_pop(pstat, stderr);
        }
        else {
            fprintf(stderr, "ERROR: couldn't write file 'monoceros.svg'\n");
        }
    }

    return 0;
}
