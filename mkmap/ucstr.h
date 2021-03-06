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

#ifndef _UCSTR_H
#define _UCSTR_H

int ucslen(uchar *ustr);
uchar *ucscpy(uchar *dest, uchar *src);
uchar *ucsncpy(uchar *dest, uchar *src, int num);
uchar *ucsntcpy(uchar *dest, uchar *src, int max);
int ucscmp(uchar *ucs1, uchar *ucs2);
uchar *ucschr(uchar *haystack, uchar needle);
uchar *ucsdup(uchar *src);
uchar *ucsndup(uchar *src, int max);
long ucstoi(uchar *ucs);
double ucstof(uchar *ucs);

#endif /* _UCSTR_H */
