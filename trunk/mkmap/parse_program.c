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

    return 0;
}

