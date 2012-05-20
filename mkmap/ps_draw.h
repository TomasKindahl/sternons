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

#ifndef _DEFS_H
#define _DEFS_H

void draw_head(progstat *pstat);
void draw_background(progstat *pstat);
void draw_grid(progstat *pstat);
void draw_stars(progstat *pstat);
void draw_line_set(progstat *pstat, uchar *id, line *line_set);
void draw_lines(progstat *pstat, uchar *id);
void draw_bounds(progstat *pstat);
void draw_delportian_area(progstat *pstat, uchar *id);
void draw_labels(progstat *pstat, uchar *NOTUSEDYET);
void draw_debug_info(progstat *pstat);
void draw_foot(progstat *pstat);
int close_file(progstat *pstat);

#endif /* _DEFS_H */
