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
#include <string.h>
#include <math.h>
#include "meta.h"
#include "usio.h"
#include "allstrings.h"
#include "ucstr.h"

#include "pointobj.h"
#include "line.h"
#include "polygon.h"
#include "label.h"
#include "projection.h"
#include "image.h"

#include "token.h"
#include "progstat.h"
#include "vmcode.h"

/* LISPIAN TOKEN LISTSTREAM */                            
/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */
typedef struct _token_liststream_S {
    int is_file;
    union {
        struct {
            struct _token_liststream_S *next;
            token *tok;
        } ref;
        token_file *tok_file;
    } V;
} token_liststream;

token_liststream *new_token_liststream(token_file *tok_file) {
    token_liststream *res = ALLOC(token_liststream);
    res->V.tok_file = tok_file;
    res->is_file = 1;
    return res;
}

token_liststream *_advance_stream(token_liststream *TS) {
    token_file *TF;
    token *tok;
    if(TS->is_file == 0) return TS;
    TF = TS->V.tok_file;
    if(tokfeof(TF)) return 0;
    tok = scan(TF);
    if(!tok) return 0;
    TS->V.ref.next = new_token_liststream(TF);
    TS->V.ref.tok = tok;
    TS->is_file = 0;
    return TS;
}

token *get_token(token_liststream *TS) {
    if(TS->is_file == 1) {
        if(!_advance_stream(TS)) return 0;
    }
    return TS->V.ref.tok;
}

token_liststream *get_next(token_liststream *TS) {
    if(TS->is_file == 1) {
        if(tokfeof(TS->V.tok_file)) return 0;
        if(!_advance_stream(TS)) return 0;
    }
    return TS->V.ref.next;
}

int expect(token_liststream **TLSP, token **tokfound, token_type type) {
    token_liststream *tlsp = *TLSP;
    token *tok = get_token(tlsp);
    char buf[1024];
    printf("expect %s (%s)\n", tok_type_str(tok), tok_str(buf, tok, 1023));
    if(is_type(tok, type)) {
        *tokfound = tok;
        *TLSP = get_next(tlsp); /* advance token liststream pointer */
        return 1;
    }
    return 0;
}

int expect_str(token_liststream **TLSP, uchar *op, token_type type) {
    token_liststream *tlsp = *TLSP;
    token *tok = get_token(tlsp);
    char buf[1024];
    printf("expect_str %s (%s)\n", tok_type_str(tok), tok_str(buf, tok, 1023));
    if(is_item(tok, op, type)) {
        *TLSP = get_next(tlsp); /* advance token liststream pointer */
        return 1;
    }
    return 0;
}

int expect_ch(token_liststream **TLSP, uchar op, token_type type) {
    token_liststream *tlsp = *TLSP;
    token *tok = get_token(tlsp);
    uchar opstr[2] = {0, 0};
    char buf[1024];
    opstr[0] = op;
    printf("expect_ch %s (%s)\n", tok_type_str(tok), tok_str(buf, tok, 1023));
    if(is_item(tok, opstr, type)) {
        *TLSP = get_next(tlsp); /* advance token liststream pointer */
        return 1;
    }
    return 0;
}

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */
/* LISPIAN TOKEN LISTSTREAM */

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

#define _VAL_NIL   0
#define _VAL_INT   1
#define _VAL_FLOAT 2
#define _VAL_USTR  3
#define _VAL_CSTR  4
#define _VAL_LIST  5

typedef struct _value_S {
    int type;
    union {
        int int_val;
        float float_val;
        uchar *ustr_val;
        char *cstr_val;
        struct {
            struct _value_S *first, *rest;
        } list_val;
    } V;
} value;

value *new_int_value(int val) {
    value *res = ALLOC(value);
    res->type = _VAL_INT;
    res->V.int_val = val;
    return res;
}

value *new_float_value(float val) {
    value *res = ALLOC(value);
    res->type = _VAL_FLOAT;
    res->V.float_val = val;
    return res;
}

value *new_uchar_value(uchar *val) {
    value *res = ALLOC(value);
    res->type = _VAL_USTR;
    res->V.ustr_val = ucsdup(val);
    return res;
}

value *new_cchar_value(char *val) {
    value *res = ALLOC(value);
    res->type = _VAL_CSTR;
    res->V.cstr_val = strdup(val);
    return res;
}

value *new_list_value(value *first, value *rest) {
    value *res = ALLOC(value);
    res->type = _VAL_LIST;
    res->V.list_val.first = first;
    res->V.list_val.rest = rest;
    return res;
}

typedef struct _def_S {
    uchar *var_name;
    value *val;
    struct _def_S *next;
} def;

def* new_def(uchar *var_name, value *val, def *next) {
    def *res = ALLOC(def);
    res->var_name = ucsdup(var_name);
    res->val = val;
    res->next = next;
    return res;
}

def* _get_value(uchar *var_name, def *def_list) {
    if(!def_list) return 0;
    if(0 == ucscmp(var_name, def_list->var_name)) {
        return def_list;
    }
    return _get_value(var_name, def_list->next);
}

value* set_value(uchar *var_name, value *new_val, def *def_list) {
    def *def_pos = _get_value(var_name, def_list);
    if(!def_pos) {
        /** error message here? */
        return 0;
    }
    /** free def_pos->val here? */
    def_pos->val = new_val;
    return new_val;
}

value* get_value(uchar *var_name, def *def_list) {
    def *def_pos = _get_value(var_name, def_list);
    if(!def_pos) {
        /** error message here? */
        return 0;
    }
    return def_pos->val;
}

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

int parse_version(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *dum;

    if(expect(&tlsp, &dum, TOK_VERSION)) {
        *TLSP = tlsp;
        return 1;
    }
    printf("Parse error: malformed version\n");
    return 0;
}

int parse_load_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *dbname, *filename;

    if(expect_str(&tlsp, u"load", TOK_ID)
    && expect(&tlsp, &dbname, TOK_ID) /* the database name */
    && expect_str(&tlsp, u"from", TOK_ID)
    && expect(&tlsp, &filename, TOK_CSTR)) /* the file name */
    {
        *TLSP = tlsp;
        return 1;
    }
    printf("Parse error: malformed load statement\n");
    return 0;
}

int parse_set_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *varname, *ustring;

    /* ⟨set statement⟩ ::= ‘set’ ⟨ID⟩ ‘=’ ⟨USTR⟩ */
    if(expect_str(&tlsp, u"set", TOK_ID)
    && expect(&tlsp, &varname, TOK_ID) /* the variable name */
    && expect_ch(&tlsp, L'=', TOK_OP)
    && expect(&tlsp, &ustring, TOK_USTR)) /* a string value */
    {
        char buf1[100], buf2[100];
        printf("VM_USTR u\"%s\" VM_IMG_SET_%s\n",
               ucstombs(buf1,tok_ustr(ustring),100),
               ucstombs(buf2,tok_ustr(varname),100));
        *TLSP = tlsp;
        return 1;
    }
    printf("Parse error: malformed set statement\n");
    return 0;

    /** TO BECOME: ⟨set statement⟩ ::= ‘set’ ⟨ID⟩ ‘=’ ⟨expression⟩ */
}

int parse_image_stmt_seq(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;

    /* ( ⟨set statement⟩ ) [ ‘;’ ⟨image sequence⟩ ] */
    /** add more image statement types!! */
    while(1) {
        if(parse_set_stmt(&tlsp)) {
            *TLSP = tlsp;
        }
        /* use 'else if' branches to detect alternative statements here */

        if(expect_ch(&tlsp, L';', TOK_OP)) {
            *TLSP = tlsp;
        }
        else {
            return 1;
        }
    }
    printf("Parse error: statement sequence\n");
    return 0;
}

int parse_image_defn(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *imgname, *momimgname;

    /* ‘image’ ⟨name⟩ ‘is’ ⟨name of inherited image⟩ ‘{’ ⟨image sequence⟩ ‘}’ */
    /** also allow a variant with no image inheritance */
    if(expect_str(&tlsp, u"image", TOK_ID)
    && expect(&tlsp, &imgname, TOK_ID) /* the image name */
    && expect_str(&tlsp, u"is", TOK_ID)
    && expect(&tlsp, &momimgname, TOK_ID) /* inherited image name */
    && expect_ch(&tlsp, L'{', TOK_LPAR)
    && parse_image_stmt_seq(&tlsp)
    && expect_ch(&tlsp, L'}', TOK_RPAR))
    {
        *TLSP = tlsp;
        return 1;
    }
    printf("Parse error: malformed image definition\n");
    return 0;
}

int parse_statement_seq(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;

    /* ( ⟨load statement⟩ | ⟨image definition⟩ ) [ ‘;’ ⟨statement sequence⟩ ] */
    /** add more statement types!! */
    while(1) {
        if(parse_load_stmt(&tlsp)) {
            *TLSP = tlsp;
        }
        else if(parse_image_defn(&tlsp)) {
            *TLSP = tlsp;
        }
        /* using 'else if' branches insert other statement kinds here */

        if(expect_ch(&tlsp, L';', TOK_OP)) {
            *TLSP = tlsp;
        }
        else {
            return 1;
        }
    }
    printf("Parse error: statement sequence\n");
    return 0;
}

int parse_main(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;

    /* ‘main’ ‘{’ ⟨statement sequence⟩ ‘}’ */
    if(expect_str(&tlsp, u"main", TOK_ID)
    && expect_ch(&tlsp, L'{', TOK_LPAR)
    && parse_statement_seq(&tlsp)
    && expect_ch(&tlsp, L'}', TOK_RPAR)
    ) {
        *TLSP = tlsp;
        return 1;
    }
    printf("Parse error: malformed main program\n");
    return 0;
}

int parse_program(token_file *TF) {
    /*char buf[1024];*/
    token_liststream *TLS = new_token_liststream(TF), *tls;

    /*for (tls = TLS; tls; tls = get_next(tls)) {
        token *tok = get_token(tls);
        uchar *ustr;
    if(!tok || is_none(tok)) break;
        for(ustr = tok_ustr(tok); *ustr; ustr++) {
            printf("%02X ", *ustr);
        }
        printf("%s (%s)\n", tok_type_str(tok), tok_str(buf, tok, 1023));
    }

    printf("================================================================="
           "===============\n");*/

    tls = TLS;
    if(parse_version(&tls) && parse_main(&tls)) {
        return 1;
    }
    printf("Parse error: malformed program\n");
    return 0;
}

void dump_program(void) {
    printf("program parsed successfully\n");
}

void usage_exit(void) {
    /* Usage text here when stabilized */
    fprintf(stderr, "Usage: parse_program /file/\n");
    exit(-1);
}

int main (int argc, char **argv) {
    token_file *TF;

    if (argc != 2) usage_exit();

    /* init: */
    if (!(TF = tokfopen(argv[1]))) {
        fprintf(stderr, "ERROR: file not found %s\n", argv[1]);
        exit(-2);
    }

    if (!parse_program(TF))
        printf("PARSE FAILED\n");
    else
        dump_program();

    tokfclose(TF);

    return 0;
}
