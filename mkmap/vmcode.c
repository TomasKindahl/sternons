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
#include "ucstr.h"
#include "meta.h"
#include "vmcode.h"

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
            printf(" %X", layer[jx]);
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