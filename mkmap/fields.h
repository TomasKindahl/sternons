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

#ifndef _FIELDS_H
#define _FIELDS_H

/** RATIONALE: specialized functions that extracts data from ‘|’ separated
        data lines from a database dump file. **/

/** DEF: functions of form next_field_TYPE, where TYPE is double, int or ustr
    ARGS: pos - a pointer to a string tape representing a value of TYPE, the
        value of TYPE is interpreted, then the pointer is pushed past to after
        next ‘|’
    RETURNS: the value of TYPE string tape pointer **/

double next_field_double(uchar **pos);
int next_field_int(uchar **pos);
/** NOTE: ustr *copies* the value at the string tape, thereby allocating enough
        memory on the heap for the copy returned. **/
uchar *next_field_ustr(uchar **pos);

#endif /* _FIELDS_H */
