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

#ifndef _PS_DRAW_H
#define _PS_DRAW_H

int draw_head(progstat *pstat);
int draw_background(progstat *pstat);
int draw_grid(progstat *pstat);
int draw_stars(progstat *pstat);
int draw_line_set(progstat *pstat, uchar *id, line *line_set);
int draw_lines(progstat *pstat, uchar *id);
int draw_bounds(progstat *pstat);
int draw_delportian_area(progstat *pstat, uchar *id);
int draw_labels(progstat *pstat, uchar *NOTUSEDYET);
int draw_debug_info(progstat *pstat);
int draw_foot(progstat *pstat);
int close_file(progstat *pstat);

#endif /* _PS_DRAW_H */
