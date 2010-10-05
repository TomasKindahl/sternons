#ifndef _PARSE_H
#define _PARSE_H

#define NO_DEBUG 0
#define DEBUG 1

typedef struct _program_state_S {
    int debug;
    FILE *debug_out;
} program_state;

program_state *new_program_state(int debug, FILE *debug_out);
int parse_program(char *program, program_state *pstat);

#endif /* _PARSE_H */
