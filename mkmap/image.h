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

#ifndef _IMAGE_H
#define _IMAGE_H

/** RATIONALE: image object and supporting methods. **/

/** RATIONALE: the image object is to contain output info pertaining to an
        SVG output generation, among others size, coloring and internal scale.
**/
typedef struct _image_S {
    uchar *name;
    int width;   /* in SVG pixels */
    int height;  /* in SVG pixels */
    int dim;     /* max(width, height) used for  */
    double scale;
    /* INSERTME: */
        /* colorings, magnitude limits, magnitude scalings ... other */
    proj *projection;
} image;

/** DEF: new_image, empty image constructor **/
image *new_image();
/** DEF: accessor methods, explanations superfluous, see above struct typedef,
        then make a C literate guess! **/
    image *IMG_set_name(image *img, uchar *name);
    image *IMG_set_size(image *img, int width, int height);
    /** NOTE: scale represents how the image is going to be “enlarged” to
        exhibit a smaller part of the sky onto the image, the larger the sky,
        the larger the “enlargement” **/
    image *IMG_set_scale(image *img, double scale);
    /** NOTE: the proj is created by calling init_Lambert (or another 
        constructor) in projection.h **/
    image *IMG_set_projection(image *img, proj *LCCP);

/** DEF: IMG_relative_pos - a function that scales a projection computed X/Y
        map position to the proper x/y place in the image
    ARGS: x - double pointer wherein to put SVG image position x coordinate,
          y - double pointer wherein to put SVG image position y coordinate,
          X - the projection computed X to readjust
          Y - the projection computed Y to readjust
          frame - the SVG image
    RETURNS: 0 if the X/Y position falls outside the image, 1 if it falls within
**/
int IMG_relative_pos(double *x, double *y, double X, double Y, image *frame);

#endif /* _IMAGE_H */
