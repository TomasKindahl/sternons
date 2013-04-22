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
#include <string.h>
#include "defs.h"
#include "usio.h"
#include "ucstr.h"
#include "meta.h"

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

int VM_init_image_program(image_program *iprog) {
    iprog->ustr.size = 16;
    iprog->ustr.arr = ALLOCN(uchar *, iprog->ustr.size);
    iprog->ustr.next = 0;

    iprog->cstr.size = 16;
    iprog->cstr.arr = ALLOCN(char *, iprog->cstr.size);
    iprog->cstr.next = 0;

    iprog->layer.size = 16;
    iprog->layer.arr = ALLOCN(optype *, iprog->layer.size);
    iprog->layer.next = 0;

    /*iprog->settings_code = 0;*/
    return 1;
}

optype *_copyops(optype *ops) {
    int ix, len = ops[0];
    optype *res;
    if(!ops) return 0;
    res = ALLOCN(optype, len+1);
    /*fprintf(stderr, "INFO: _copyops(optype, %i);\n", len+1);*/
    for(ix=0; ix<=len; ix++) {
        res[ix] = ops[ix];
    }
    return res;
}

int VM_add_code_layer(image_program *iprog, optype *ops) {
    /* Check for overflow here! */
    iprog->layer.arr[iprog->layer.next++] = _copyops(ops);
    iprog->layer.arr[iprog->layer.next] = 0;
    return 1;
}

int VM_add_ustring(image_program *iprog, uchar *ustr) {
    iprog->ustr.arr[iprog->ustr.next++] = ucsdup(ustr);
    iprog->ustr.arr[iprog->ustr.next] = 0;
    return 1;
}

int VM_add_cstring(image_program *iprog, char *cstr) {
    iprog->cstr.arr[iprog->cstr.next++] = strdup(cstr);
    iprog->cstr.arr[iprog->cstr.next] = 0;
    return 1;
}

int VM_dump_image_program(image_program *iprog) {
    int ix, jx;

    printf("\nImage dump:\n");
    for (ix = 0; ix < iprog->cstr.next; ix++) {
        char *cstr = iprog->cstr.arr[ix];
        printf("  1 %X", cstr[0]);
        for (jx = 1; cstr[jx]; jx++) {
            printf(" %X", cstr[jx]);
        }
        printf("\n");
    }
    for (ix = 0; ix < iprog->ustr.next; ix++) {
        uchar *ustr = iprog->ustr.arr[ix];
        printf("  2 %X", ustr[0]);
        for (jx = 1; ustr[jx]; jx++) {
            printf(" %X", ustr[jx]);
        }
        printf("\n");
    }
    for (ix = 0; ix < iprog->layer.next; ix++) {
        optype *layer = iprog->layer.arr[ix];
        int len = layer[0];
        printf("  %X %X", ix+3, len);
        for (jx = 1; jx <= len; jx++) {
            printf(" %X", (unsigned int)layer[jx]);
        }
        printf("\n");
    }
    return 1;
}

int VM_add_ustring_length(image_program *iprog, int len, int *ustr) {
    uchar *ustr_w_null = ucsndup(ustr, len);
    ustr_w_null[len] = '\0';
    iprog->ustr.arr[iprog->ustr.next++] = ustr_w_null;
    iprog->ustr.arr[iprog->ustr.next] = 0;
    return 1;
}

int VM_add_cstring_from_ustring(image_program *iprog, int len, int *ustr) {
    char *cstr = ALLOCN(char,len+1);
    int ix;
    for(ix = 0; ix < len; ix++) cstr[ix] = ustr[ix];
    cstr[ix] = '\0';
    /*printf("--- ");
    for(ix = 0; ix < len+1; ix++) printf("%X ", cstr[ix]);
    printf("\n");*/
    iprog->cstr.arr[iprog->cstr.next++] = cstr;
    iprog->cstr.arr[iprog->cstr.next] = 0;
    return 1;
}

int VM_add_code_layer_length(image_program *iprog, int len, int *ops) {
    int ix;
    optype *res;
    if(!ops) return 0;
    res = ALLOCN(optype, len+1);
    res[0] = len;
    for(ix = 0; ix < len; ix++) {
        res[ix+1] = ops[ix];
    }
    iprog->layer.arr[iprog->layer.next++] = res;
    iprog->layer.arr[iprog->layer.next] = 0;
    return 1;
}

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
    if(0 == strcmp(cstr,"OpenFile")) return VM_OPEN_FILE;
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
    /*char *fname = PS_get_cstr(pstat);*/
    pstat = PS_push(pstat, stderr);
    for (ix = 0; iprog->layer.arr[ix]; ix++)
        VM_do(iprog, ix, pstat);
    pstat = PS_pop(pstat, stderr);
    return 1;
}

