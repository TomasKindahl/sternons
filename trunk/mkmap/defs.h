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

#ifndef _DEFS_H
#define _DEFS_H

/** RATIONALE: type definitions that have some generic area of application,
        but who are specific for sternons related programs **/

/** DEF: uchar as a 4 byte character
    RATIONALE: wchar of Windows is only 2 byte, making Unicode ≥ Plane 1
        impossible.
    OTHER: the libs ucstr, uctype and usio provides some functions **/
typedef int uchar;

#endif /* _DEFS_H */
