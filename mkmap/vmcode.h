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

#ifndef _VMCODE_H
#define _VMCODE_H

#include "meta.h"
#include "defs.h"

#define VM_NULL                  0x00

#define VM_CSTR                  0x01 /* up to VM_NULL */
#define VM_USTR                  0x02 /* up to VM_NULL */
#define VM_INT                   0x03 /* 1 int */
#define VM_DBL                   0x04 /* 2 ints */

#define VM_BEGIN                 0x10
#define VM_END                   0x11
#define VM_LOAD_STARS            0x12
#define VM_LOAD_STAR_LINES       0x13
#define VM_LOAD_CONST_BOUNDS     0x14
#define VM_NEW_IMAGE             0x15
#define VM_IMG_SET_NAME          0x17
#define VM_IMG_SET_SIZE          0x18
#define VM_IMG_SET_SCALE         0x19
#define VM_IMG_SET_LAMBERT       0x1A
#define VM_OPEN_FILE             0x1B
#define VM_LOAD_LABELS           0x1C
#define VM_DRAW_DELPORTIAN_AREA  0x1D
#define VM_DRAW_LINES            0x1E
#define VM_DRAW_LABELS           0x1F
#define VM_DRAW_HEAD             0x20
#define VM_DRAW_BACKGROUND       0x21
#define VM_DRAW_BOUNDS           0x22
#define VM_DRAW_GRID             0x23
#define VM_DRAW_STARS            0x24
#define VM_DRAW_DEBUG_INFO       0x25
#define VM_DRAW_FOOT             0x26
#define VM_CLOSE_FILE            0x27

#define VM_CSTR_STORE            0x01
#define VM_USTR_STORE            0x02
#define VM_SETTINGS_LAYER        0x03
#define VM_IMAGE_DATA_LAYER      0x04
#define VM_INIT_DRAWING_LAYER    0x05
#define VM_SUPPORT_DRAWING_LAYER 0x06
#define VM_REAL_OBJECTS_LAYER    0x07
#define VM_FINAL_LAYER           0x08

typedef long int optype;

typedef struct _image_program_S {
    DEFARR(char *, cstr);
    DEFARR(uchar *, ustr);
    DEFARR(optype *, layer);
} image_program;

int VM_init_image_program(image_program *iprog);
int VM_add_code_layer(image_program *iprog, optype *ops);
int VM_add_ustring(image_program *iprog, uchar *ustr);
int VM_add_cstring(image_program *iprog, char *cstr);
int VM_dump_image_program(image_program *iprog);

int VM_add_ustring_length(image_program *iprog, int len, int *ustr);
int VM_add_cstring_from_ustring(image_program *iprog, int len, int *ustr);
int VM_add_code_layer_length(image_program *iprog, int len, int *ops);

#endif /* _VMCODE_H */
