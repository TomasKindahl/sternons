/************************************************************************
 *  Copyright (C) 2010-203 Tomas Kindahl                               *
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

#ifndef _PS_DB_H
#define _PS_DB_H

void sort_pointobj_view(progstat *pstat, VIEW(pointobj) *view,
                        int compare(const void *, const void *));
int load_stars(progstat *pstat, char *fname);
pointobj *find_star_by_HIP(int HIP, progstat *pstat);
int load_star_lines(progstat *pstat, char *fname);
int load_constellation_bounds(progstat *pstat, char *fname);
int load_star_labels(progstat *pstat, char *fname);

#endif /* _PS_DB_H */
