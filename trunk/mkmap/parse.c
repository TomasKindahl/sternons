/************************************************************************
 *  Copyright (C) 2010 Tomas Kindahl                                    *
 ************************************************************************
 *  This file is part of mkmap                                          *
 *                                                                      *
 *  mkmap is free software: you can redistribute it and/or modify it    *
 *  under the terms of the GNU Lesser General Public License as         *
 *  published by the Free Software Foundation, either version 3 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *  mkmap is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *  GNU Lesser General Public License for more details.                 *
 *                                                                      *
 *  You should have received a copy of the GNU Lesser General Public    *
 *  License along with mkmap. If not, see                               *
 *  <http://www.gnu.org/licenses/>.                                     *
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "usio.h"
#include "ucstr.h"
#include "token.h"
#include "parse.h"

void tok_dump(token *tok, program_state *pstat) {
    char buf[1024], buf2[1024];
    uchar _L_0x3B[] = {L';',0};
    static num = 0;

    switch (tok->type) {
      case TOK_STR:
        fprintf(pstat->debug_out, "⟨%s⟩“%s”", tok_type_str(tok), tok_str(buf, tok, 1023));
        break;
      case TOK_NUM:
        if (tok->base == BAS_DEC)
            fprintf(pstat->debug_out, "⟨%s⟩%s(%s)", tok_type_str(tok),
                    tok_str(buf, tok, 1023), tok_unit(buf2, tok, 1023));
        else
            fprintf(pstat->debug_out, "⟨%s⟩%s(%s/%s)", tok_type_str(tok),
                    tok_str(buf, tok, 1023), tok_unit(buf2, tok, 1023),
                    tok_base_name(tok));
        break;
      case TOK_OP:
          if (is_op(tok, _L_0x3B))
              fprintf(pstat->debug_out, "⟨OP⟩;\n");
        else
              fprintf(pstat->debug_out, "⟨OP⟩%s", tok_str(buf, tok, 1023));
        break;
      default:                    
        fprintf(pstat->debug_out, "⟨%s⟩%s", tok_type_str(tok), tok_str(buf, tok, 1023));
    }
    if (num == 7) {
        fprintf(pstat->debug_out, "\n  ⊕ ");
        num = 0;
    }
    else num++;
}

program_state *new_program_state(int debug, FILE *debug_out) {
    program_state *res = (program_state *)malloc(sizeof(program_state));
    res->debug = debug;
    res->debug_out = debug_out;
    return res;
}

/*************************************************************/
/**                    PROGRAM SETTINGS                     **/
/*************************************************************/

void set_program_attr(token *var, token *attr, token *value, program_state *pstat) {
    return;
}

void set_program_var(token *var, token *value, program_state *pstat) {
    char buf[1024], buf2[1024];
    uchar _L_name[] = {'n','a','m','e',0};
    uchar _L_width[] = {'w','i','d','t','h',0};
    uchar _L_height[] = {'h','e','i','g','h','t',0};
    uchar _L_scale[] = {'s','c','a','l','e',0};
    
    if (is_kw(var, _L_name)) {
        /* CHECK that value is TOK_STR HERE! */
        /* pstat->image->name = ucsdup(tok_ustr(value));
        fprintf(pstat->debug_out, " [name = “%s”]", ucstombs(buf, pstat->image->name, 1023));*/
        return;
    }
    if (is_kw(var, _L_width)) {
        int w, h, d;
        /* CHECK that value is TOK_NUM HERE! */
        /* pstat->image->width = ucstoi(tok_ustr(value));
        w = pstat->image->width; h = pstat->image->height;
        if(w > h) d = w; else d = h; pstat->image->dim = d;
        fprintf(pstat->debug_out, " [width = %i]", pstat->image->width);*/
        return;
    }
    if (is_kw(var, _L_height)) {
        int w, h, d;
        /* CHECK that value is TOK_NUM HERE! */
        /* pstat->image->height = ucstoi(tok_ustr(value));
        w = pstat->image->width; h = pstat->image->height;
        if(w > h) d = w; else d = h; pstat->image->dim = d;
        fprintf(pstat->debug_out, " [height = %i]", pstat->image->height);*/
        return;
    }
    if (is_kw(var, _L_scale)) {
        /* CHECK that value is TOK_NUM HERE! */
        /* pstat->image->scale = ucstof(tok_ustr(value));
        fprintf(pstat->debug_out, " [scale = %f]", pstat->image->scale);*/
        return;
    }
    fprintf(pstat->debug_out, " [WARNING: unused assignment %s = “%s”]",
            ucstombs(buf, tok_ustr(var), 1023),
            ucstombs(buf2, tok_ustr(value), 1023));
    return;
}

/*************************************************************/
/**                    PROGRAM PARSING                      **/
/*************************************************************/

/**************/
/** SCANNING **/
/**************/

token *scan_value(token_file *pfile, program_state *PS) {
    token *tok;
    if (tokfeof(pfile)) return 0;
    tok = scan(pfile);
    tok_dump(tok, PS);
    return tok;
}

token *scan_item(token_file *pfile, token_type type, program_state *PS) {
    token *tok;
    if (tokfeof(pfile)) return 0;
    tok = scan(pfile);
    if (!is_type(tok, type)) {
        unscan(tok, pfile);
        return 0;
    }
    tok_dump(tok, PS);
    return tok;
}

token *scan_exact(token_file *pfile, token_type type, uchar *val, program_state *PS) {
    token *tok = scan_item(pfile, type, PS);
    if (!tok) return 0;
    if (!is_str(tok, val)) {
        unscan(tok, pfile);
        return 0;
    }
    return tok;
}

/*************/
/** PARSING **/
/*************/

void _parse_program_head(token_file *pfile, program_state *pstat) {
    token *tok;
    int NL = 0;
    uchar _L_image[] = {'i','m','a','g','e', 0}; 

    tok = scan(pfile);
    fprintf(pstat->debug_out, "INFO: head reading {\n    ");
    while (!is_kw(tok, _L_image)) {
        tok_dump(tok, pstat);
        tok = scan(pfile);
        if (NL == 8)
            { fprintf(pstat->debug_out, "\n    "); NL = 0; }
        else
            { fprintf(pstat->debug_out, " "); NL++; }
    }
    fprintf(pstat->debug_out, "\n}\n");
    unscan(tok, pfile);
    return;
}

void _parse_program_assignment(token_file *pfile, program_state *pstat) {
    token *tok, *var, *attr, *value;
    uchar _L_0x7D[] = {'}',0};
    uchar _L_0x2E[] = {'.',0};
    uchar _L_0x3D[] = {'=',0};

    if(pstat->debug) fprintf(pstat->debug_out, "  INFO: assignment {\n      ");
    var = scan(pfile); tok_dump(var, pstat);
    if (is_rpar(var, _L_0x7D)) { unscan(var, pfile); return; }
    tok = scan(pfile); tok_dump(tok, pstat);
    if (is_op(tok,_L_0x2E)) { /* Attribute assignment: */
        attr  = scan_item(pfile, TOK_KW, pstat);
        tok   = scan_exact(pfile, TOK_OP, _L_0x3D, pstat);
        value = scan_value(pfile, pstat);
        set_program_attr(var, attr, value, pstat);

        if (pstat->debug) fprintf(pstat->debug_out, " (attrib)\n");
    }
    else if (is_op(tok,_L_0x3D)) { /* Variable assignment: */
        value = scan_value(pfile, pstat);
        set_program_var(var, value, pstat);
        if (pstat->debug) fprintf(pstat->debug_out, " (variable)\n");
    }
    else { /* Shouldn't occur here! */
        fprintf(pstat->debug_out, "  ERROR: error in assignment\n");
        unscan(tok, pfile); return;
    }
    fprintf(pstat->debug_out, "  }\n");
}

void _parse_program_image(token_file *pfile, program_state *pstat) {
    token *tok;
    uchar _L_0x7B[] = {'}',0};

    tok = scan(pfile); tok_dump(tok, pstat);     /* image */
    tok = scan(pfile); tok_dump(tok, pstat);     /* { */
    fprintf(pstat->debug_out, "\nINFO: image reading {\n");
    while (!is_rpar(tok, _L_0x7B)) {
        _parse_program_assignment(pfile, pstat);
        tok = scan(pfile);
    }
    fprintf(pstat->debug_out, "\n}\n");
    return;
}

int parse_block_statement(token_file *pfile, program_state *pstat) {
    token *tok;
    uchar _C_lbrace[] = {'{',0}, _C_rbrace[] = {'}',0};
    FILE *dout = pstat->debug_out;

    if(tokfeof(pfile)) return 0;
    if (!is_lpar(tok = scan(pfile), _C_lbrace))
        { unscan(tok, pfile); return 0; }
    fprintf(dout, "\n**** { ****\n");
    while (!is_rpar(tok, _C_rbrace)) {
        tok_dump(tok, pstat);
        if(tokfeof(pfile)) return 0;
        tok = scan(pfile);
    }
    tok_dump(tok, pstat);
    fprintf(dout, "\n**** } ****\n");
    return 1;
}

int parse_undefined_statement(token_file *pfile, program_state *pstat) {
    token *tok;
    uchar _C_semi[] = {';',0}, _C_rbrace[] = {'}',0};
    FILE *dout = pstat->debug_out;

    if(tokfeof(pfile)) return 0;
    if (is_rpar(tok = scan(pfile), _C_rbrace))
        { unscan(tok, pfile); return 0; }
    while (!is_op(tok, _C_semi)) {
        tok_dump(tok, pstat);
        if(tokfeof(pfile)) return 0;
        tok = scan(pfile);
    }
    tok_dump(tok, pstat);
    return 1;
}

int parse_procedure(token_file *pfile, program_state *pstat) {
    char buf[1024];
    token *name, *tok;
    uchar _C_lpar[]   = {'(',0}, _C_rpar[]   = {')',0};
    uchar _C_lbrace[] = {'{',0}, _C_rbrace[] = {'}',0};
    FILE *dout = pstat->debug_out;
    int end;

    if(tokfeof(pfile)) return 0;
    if (!is_any_kw(name = scan(pfile))) {
        if (tokfeof(pfile)) return 0;
        fprintf(dout, "ERROR: function name expected on line %i, instead "
                "got '%s'\n", name->line_num, ucstombs(buf, tok_ustr(name), 1023));
        return 0;
    }
    if(tokfeof(pfile)) return 0;
    if (!is_lpar(tok = scan(pfile),_C_lpar)) {
        fprintf(dout, "ERROR: '(' expected on line %i\n", name->line_num);
        return 0;
    }
    /*************************/
    /** ARGUMENT LIST HERE! **/
    /*************************/
    if(tokfeof(pfile)) return 0;
    if (!is_rpar(tok = scan(pfile),_C_rpar)) {
        fprintf(dout, "ERROR: ')' expected on line %i\n", name->line_num);
        return 0;
    }
    if(tokfeof(pfile)) return 0;
    if (!is_lpar(tok = scan(pfile),_C_lbrace)) {
        fprintf(dout, "ERROR: '{' expected on line %i\n", name->line_num);
        return 0;
    }
    if(pstat->debug == DEBUG) {
        fprintf(dout, "\n**** %s () { ****\n", ucstombs(buf, tok_ustr(name), 1023));
    }
    for (end = 1; end; ) {
        if (parse_block_statement(pfile, pstat)) {
            ;
        }
        else if (parse_undefined_statement(pfile, pstat)) {
            ;
        }
        else {
            end = 0;
        }
    }
    if(tokfeof(pfile)) return 0;
    if (!is_rpar(tok = scan(pfile),_C_rbrace)) {
        fprintf(dout, "ERROR: '}' expected on line %i\n", name->line_num);
        return 0;
    }
    if(pstat->debug == DEBUG) { 
        fprintf(dout, "\n**** } ****\n");
    }
    if(tokfeof(pfile)) return 0;
    return 1;
}

int parse_program(char *program, program_state *pstat) {
    token_file *pfile;
    token *tok;
    int NL;

    if (!(pfile = tokfopen(program))) {
        fprintf(pstat->debug_out, "ERROR: program '%s' not found\n", program);
        return 0;
    }
    fprintf(pstat->debug_out, "INFO: program '%s' opened\n", program);
    if (pstat->debug == DEBUG) {
        NL = 0;
        while (!tokfeof(pfile)) {
            if (!parse_procedure(pfile, pstat)) {
                if (tokfeof(pfile)) return 1;
                fprintf(pstat->debug_out, "\nERROR: procedure parse failed\n");
                break;
            }
        }
    }

    fprintf(pstat->debug_out, "\nINFO: program '%s' parsed\n", program);
    tokfclose(pfile);
    return 1;
}


