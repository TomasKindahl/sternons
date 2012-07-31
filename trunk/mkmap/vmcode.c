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

#include <stdlib.h>
#include <stdio.h>
#include "meta.h"
#include "vmcode.h"

int VM_init_image_program(image_program *iprog) {
    iprog->_layer_size = 16;
    iprog->layer = ALLOCN(optype *, iprog->_layer_size);
    iprog->_layer_next = 0;
    /*iprog->settings_code = 0;*/
    return 1;
}

optype *_copyops(optype *ops) {
    int ix, len = ops[0];
    optype *res;
    if(!ops) return 0;
    res = ALLOCN(optype, len+1);
    /*fprintf(stderr, "INFO: _copyops(optype, %i);\n", len+1);*/
    for(ix=0; ix<=len; ix++) {
        res[ix] = ops[ix];
    }
    return res;
}

int VM_add_code_layer(image_program *iprog, optype *ops) {
    /* Check for overflow here! */
    iprog->layer[iprog->_layer_next++] = _copyops(ops);
    iprog->layer[iprog->_layer_next] = 0;
    return 1;
}
