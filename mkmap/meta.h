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

#ifndef _META_H
#define _META_H

/* Arithmetic */
#define BETW(LB,X,UB) (((LB)<(X))&&((X)<(UB)))

/* Allocation */
#define ALLOC(TYPE) (TYPE *)malloc(sizeof(TYPE))
#define ALLOCN(TYPE,NUM) (TYPE *)malloc(sizeof(TYPE)*(NUM))

/* Database sorted view of an item list */
#define _VIEW(ITEM) struct _ ## ITEM ## _view_S
#define VIEW(ITEM) ITEM ## _view
#define DEFVIEW(ITEM)        \
    typedef _VIEW(ITEM) {    \
        int size;            \
        int next;            \
        ITEM **S;            \
    } VIEW(ITEM)
#define DEFARR(TYPE,NAME)    \
    struct {                \
        int size;            \
        int next;            \
        TYPE *arr;          \
    } NAME

#endif /* _META_H */
