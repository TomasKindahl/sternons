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

#include "defs.h"
#include "uctype.h"

/* uchar is White Space? */
int isuws(uchar uch) {
    return (uch == L' ') || (uch == L'\t') || (uch == L'\n');
}

    /* uchar is alphabetic char? (only Latin considered yet) */
int isualpha(uchar uch) {
    if (L'_' == uch) return 1;
    /* ASCII */
    if (L'A' <= uch && uch <= L'Z') return 1;
    if (L'a' <= uch && uch <= L'z') return 1;
    /* Latin-1, -A, -B and IPA */
    if (L'À' <= uch && uch <= L'ʯ' && uch != L'×' && uch != L'÷') return 1;
    /* Spacing modifier letters */
    if (L'ʰ' <= uch && uch <= L'˿') return 1;
    /* Greek */
    if (L'Ά' <= uch && uch <= L'Ͽ' && uch != L'·') return 1;
    return 0;
}

int isuhex(uchar uch) {
    return ('0' <= uch && uch <= '9') || ('A' <= uch && uch <= 'F')
        || ('a' <= uch && uch <= 'f');
}

    /* uchar is digit? (only Latin considered yet) */
int isunum(uchar uch) {
    if (L'0' <= uch && uch <= L'9') return 1;
    return 0;
}

int isualnum(uchar uch) {
    return isualpha(uch) || isunum(uch);
}

int isualusc(uchar uch) {
    return isualpha(uch) || (uch == '_');
}

/* uchar string functions (cygwin has a UTF-16 wchar_t, we assume UTF-32 - int) */
