/************************************************************************
 *  Copyright (C) 2010 Tomas Kindahl                                    *
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
#include <string.h>
#include <math.h>
#include "usio.h"
#include "ucstr.h"
#include "token.h"
#include "parse.h"

void usage_exit(void) {
    /* Usage text here when stabilized */
    fprintf(stderr, "Usage: parse_program /file/\n");
    exit(-1);
}

int main (int argc, char **argv) {
    program_state *pstat = new_program_state(DEBUG, stdout);

    if (argc != 2) usage_exit();

    /* init: */
    if (!parse_program(argv[1], pstat))
        usage_exit();
    else
        dump_program(pstat);

    return 0;
}

