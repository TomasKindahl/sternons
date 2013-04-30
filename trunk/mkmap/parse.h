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

#ifndef _PARSE_H
#define _PARSE_H

#define NO_DEBUG 0
#define DEBUG 1

typedef struct _map_layer_S {
    uchar *name;
    struct _map_layer_S *next;
} map_layer;

typedef struct _procedure_S {
    uchar *name;
    map_layer *layers;
    struct _procedure_S *next;
} procedure;

typedef struct _program_state_S {
    int debug;
    FILE *debug_out;
    procedure *proc;
} program_state;

program_state *new_program_state(int debug, FILE *debug_out);
int parse_program(char *program, program_state *pstat);
void dump_program(program_state *pstat);

#endif /* _PARSE_H */

