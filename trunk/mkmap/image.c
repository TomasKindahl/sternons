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

#include <stdlib.h>
#include "meta.h"
#include "defs.h"
#include "ucstr.h"
#include "projection.h"
#include "image.h"

image *new_image() {
    image *res = ALLOC(image);
    res->name = 0;
    res->width = 0;
    res->height = 0;
    res->scale = 0;
    return res;
}

image *IMG_set_name(image *img, uchar *name) {
    img->name = name;
    return img;
}

image *IMG_set_size(image *img, int width, int height) {
    img->width = width;
    img->height = height;
    if(width > height) img->dim = width; else img->dim = height;
    return img;
}

image *IMG_set_scale(image *img, double scale) {
    img->scale = scale;
    return img;
}

image *IMG_set_projection(image *img, proj *LCCP) {
    img->projection = LCCP;
    return img;
}

int IMG_relative_pos(double *x, double *y, double X, double Y, image *frame) {
    *x = frame->width/2-frame->dim*X*frame->scale;
    *y = frame->height/2-frame->dim*Y*frame->scale;
    return BETW(0,*x,frame->width) && BETW(0,*y,frame->height);
}
