/************************************************************************
 *  Copyright (C) 2010 Tomas Kindahl                                    *
 ************************************************************************
 *  This file is part of mkmap                                          *
 *                                                                      *
 *  mkmap is free software: you can redistribute it and/or modify it    *
 *  under the terms of the GNU Lesser General Public License as         *
 *  published by the Free Software Foundation, either version 3 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *  mkmap is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *  GNU Lesser General Public License for more details.                 *
 *                                                                      *
 *  You should have received a copy of the GNU Lesser General Public    *
 *  License along with mkmap. If not, see                               *
 *  <http://www.gnu.org/licenses/>.                                     *
 ************************************************************************/

#ifndef _USIO_H
#define _USIO_H

#include "defs.h"

#define REPLACEMENT_CHAR 0xFFFD

typedef struct _utf8_file_S {
    /* specialized token file allowing token get and unget */
    FILE *chfile;
    uchar uchar_save;
    int line_num;
} utf8_file;

utf8_file *u8fopen(char *fname);        /* DO: "rt", "wt" */
int u8fclose(utf8_file *file);
int u8feof(utf8_file *file);
uchar fgetuc(utf8_file *stream);
uchar fungetuc(int uch, utf8_file *stream);
uchar *fgetus(uchar *us, int size, utf8_file *stream);
int u8flineno(utf8_file *file);

/* DO: fputuc, fputus */
char *uctombs(char *dest, uchar src);
char *ucstombs(char *dest, uchar *src, int size);

#endif /* _USIO_H */
