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

#define VM_NULL                   0

#define VM_CSTR                   1 /* up to VM_NULL */
#define VM_USTR                   2 /* up to VM_NULL */
#define VM_INT                    3 /* 1 int */
#define VM_DBL                    4 /* 2 ints */

#define VM_BEGIN                 10
#define VM_END                   11
#define VM_LOAD_STARS            12
#define VM_LOAD_STAR_LINES       13
#define VM_LOAD_CONST_BOUNDS     14
#define VM_NEW_IMAGE             15
#define VM_IMG_SET_NAME          17
#define VM_IMG_SET_SIZE          18
#define VM_IMG_SET_SCALE         19
#define VM_IMG_SET_LAMBERT       20
#define VM_OPEN_FILE             21
#define VM_LOAD_LABELS           22
#define VM_DRAW_DELPORTIAN_AREA  23
#define VM_DRAW_LINES            24
#define VM_DRAW_LABELS           25
#define VM_DRAW_HEAD             26
#define VM_DRAW_BACKGROUND       27
#define VM_DRAW_BOUNDS           28
#define VM_DRAW_GRID             29
#define VM_DRAW_STARS            30
#define VM_DRAW_DEBUG_INFO       31
#define VM_DRAW_FOOT             32
#define VM_CLOSE_FILE            33

#define VM_SETINGS_LAYER         0
#define VM_IMAGE_DATA_LAYER      1
#define VM_INIT_DRAWING_LAYER    2
#define VM_SUPPORT_DRAWING_LAYER 3
#define VM_REAL_OBJECTS_LAYER    4
#define VM_FINAL_LAYER           5

typedef long int optype;

typedef struct _image_program_S {
    int _layer_size, _layer_next;
    optype **layer;
} image_program;

int VM_init_image_program(image_program *iprog);
int VM_add_code_layer(image_program *iprog, optype *ops);

#endif /* _VMCODE_H */
