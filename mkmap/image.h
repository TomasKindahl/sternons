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

#ifndef _IMAGE_H
#define _IMAGE_H

typedef struct _image_S {
    uchar *name;
    int width, height, dim;
    double scale;
    /* INSERTME: */
        /* colorings, magnitude limits, magnitude scalings ... other */
    proj *projection;
} image;

image *new_image();
image *IMG_set_name(image *img, uchar *name);
image *IMG_set_size(image *img, int width, int height);
image *IMG_set_scale(image *img, double scale);
image *IMG_set_projection(image *img, proj *LCCP);
int IMG_relative_pos(double *x, double *y, double X, double Y, image *frame);

#endif /* _IMAGE_H */

