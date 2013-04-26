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
/*#include "progstat.h"*/
/*#include "vmcode.h"*/

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

void _dump_token_liststream(token_liststream *TS) {
    token *tok; char buf[1024];
    if(!TS) {
        printf(")\n"); fflush(stdout);
        return;
    }
    if(TS->is_file) {
        printf("file "); fflush(stdout);
    }
    else {
        tok = TS->V.ref.tok;
        printf("%s:",tok_type_str(tok));
        switch(tok->type) {
          case TOK_NONE:
             printf("(none) "); fflush(stdout); break;
          case TOK_ID:
          case TOK_LPAR:
          case TOK_NUM:
          case TOK_OP:
          case TOK_RPAR:
          case TOK_USTR:
          case TOK_VERSION:
            printf("%s ", tok_str(buf, tok, 1024)); fflush(stdout); break;
          case TOK_CSTR:
            printf("%s ", tok_str(buf, tok, 1024)); fflush(stdout); break;
          default:
            printf("(fel) "); fflush(stdout); break;
        }
    }
    _dump_token_liststream(TS->V.ref.next);
}

void dump_token_liststream(token_liststream *TS) {
    printf("("); _dump_token_liststream(TS);
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
    opstr[0] = op;
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

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

FILE *output_file;

char *new_ext(char *infname, char *new_ext) {
    char *res, *pos;
    int len = strlen(infname);
    pos = strrchr(infname, '.');
    if(pos) len = pos-infname;
    res = ALLOCN(char, len+1+strlen(new_ext)+1);
    strncpy(res, infname, len);
    res[len] = '.';
    strcpy(&res[len+1], new_ext);
    return res;
}

/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

/* ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

#define VM_SETTINGS_LAYER        0x00
#define VM_IMAGE_DATA_LAYER      0x01
#define VM_INIT_DRAWING_LAYER    0x02
#define VM_SUPPORT_DRAWING_LAYER 0x03
#define VM_REAL_OBJECTS_LAYER    0x04
#define VM_FINAL_LAYER           0x05

typedef struct _strlis_S {
    char *str;
    struct _strlis_S *next;
} strlis;

strlis *new_strlis(char *str, strlis *next) {
    strlis *res = ALLOC(strlis);
    res->str = str;
    res->next = next;
    return res;
}

void dump_strlis(strlis *SL) {
    if(!SL) return;
    dump_strlis(SL->next);
    fprintf(output_file, " %s", SL->str);
}

typedef struct _code_S {
    char **cstr;
    int cstr_sz, cstr_nx;
    uchar **ustr;
    int ustr_sz, ustr_nx;
    strlis *vm[6];
    int vm_current;
} code;

code *the_code;

code *new_code(void) {
    const int size = 16;
    int ix;
    code *res = ALLOC(code);
    res->cstr = ALLOCN(char *, size); res->cstr_sz = size; res->cstr_nx = 0;
    res->ustr = ALLOCN(uchar *, size); res->ustr_sz = size; res->ustr_nx = 0;
    for(ix = 0; ix < 6; ix++)
       res->vm[ix] = 0;
    res->vm_current = VM_SETTINGS_LAYER;
    return res;
}

int add_ustr(code *the_code, uchar *ustr) {
    int ix = the_code->ustr_nx;
    the_code->ustr[the_code->ustr_nx++] = ucsdup(ustr);
    return ix;
}

int add_cstr(code *the_code, char *cstr) {
    int ix = the_code->cstr_nx;
    the_code->cstr[the_code->cstr_nx++] = strdup(cstr);
    return ix;
}

void add_code_str(code *the_code, char *str) {
    strlis *SL = the_code->vm[the_code->vm_current];
    SL = new_strlis(strdup(str), SL);
    the_code->vm[the_code->vm_current] = SL;
}

void add_code_num(code *the_code, int num) {
    strlis *SL = the_code->vm[the_code->vm_current];
    char buf[128];
    sprintf(buf, "%X", num);
    SL = new_strlis(strdup(buf), SL);
    the_code->vm[the_code->vm_current] = SL;
}

void add_code_int(code *the_code, token *numtok) {
    add_code_str(the_code, ":int");
    add_code_num(the_code, tok_int(numtok));
}

typedef struct _int2_S {
   int I1, I2;
} int2;

int2 to_int2(double D) {
	union {
		double D;
		int2 L;
	} res;
	res.D = D;
	return res.L;
}

void add_code_real(code *the_code, int sign, token *numtok) {
    int2 L;
    if(sign == -1)
        L = to_int2(-tok_real(numtok));
    else
        L = to_int2(tok_real(numtok));
    add_code_str(the_code, ":dbl");
    add_code_num(the_code, L.I2);
    add_code_num(the_code, L.I1);
}

void add_code_cstr_ix(code *the_code, int index) {
    add_code_str(the_code, ":cstr");
    add_code_num(the_code, index);
}

void add_code_ustr_ix(code *the_code, int index) {
    add_code_str(the_code, ":ustr");
    add_code_num(the_code, index);
}

void add_code_NL(code *the_code) {
    strlis *SL = the_code->vm[the_code->vm_current];
    char buf[] = "\n  ";
    SL = new_strlis(strdup(buf), SL);
    the_code->vm[the_code->vm_current] = SL;
}

void dump_code(code *the_code) {
    char *label[] = {
      "SETTINGS", "IMAGE_DATA", "INIT_DRAWING", 
      "SUPPORT_DRAWING", "REAL_OBJECTS", "FINAL"
    };
    int ix, jx;
    fprintf(output_file, "mbf1\n");
    for(ix = 0; ix < the_code->cstr_nx; ix++) {
        fprintf(output_file, ":CSTR");
        for(jx = 0; the_code->cstr[ix][jx] != 0; jx++)
            fprintf(output_file, " %02X", the_code->cstr[ix][jx]);
        fprintf(output_file, " -\n");
    }
    for(ix = 0; ix < the_code->ustr_nx; ix++) {
        fprintf(output_file, ":USTR");
        for(jx = 0; the_code->ustr[ix][jx] != 0; jx++)
            fprintf(output_file, " %02X", the_code->ustr[ix][jx]);
        fprintf(output_file, " -\n");
    }
    for(ix = 0; ix < 6; ix++) {
        fprintf(output_file, ":%s\n  ", label[ix]);
        dump_strlis(the_code->vm[ix]);
        fprintf(output_file, " -\n");
    }
}
/* ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ */

int parse_version(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *dum;

    /** TODO: use version number for selecting correct parser */
    if(expect(&tlsp, &dum, TOK_VERSION)) {
        *TLSP = tlsp;
        return 1;
    }
    printf("Parse error: malformed version\n");
    return 0;
}

int parse_name_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *name;
    int ix;

    /* ⟨name statement⟩ ::= ‘name’ ⟨USTR⟩ */
    if(expect_str(&tlsp, u"name", TOK_ID)
    && expect(&tlsp, &name, TOK_USTR)) /* a string value */
    {
        ix = add_ustr(the_code, tok_ustr(name));
        add_code_ustr_ix(the_code, ix);
        add_code_str(the_code, ":ImgSetName");
        add_code_NL(the_code);
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_size_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *num1, *num2;

    /* ⟨size statement⟩ ::= ‘size’ ⟨NUM⟩ ⟨NUM⟩ */
    if(expect_str(&tlsp, u"size", TOK_ID)
    && expect(&tlsp, &num1, TOK_NUM)  /* a string value */
    && expect(&tlsp, &num2, TOK_NUM)) /* a string value */
    {
        add_code_int(the_code, num1);
        add_code_int(the_code, num2);
        add_code_str(the_code, ":ImgSetSize");
        add_code_NL(the_code);
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_constexpr(token_liststream **TLSP, int *numsign, token **num) {
    token_liststream *tlsp = *TLSP;
    *numsign = 0;
    if(expect_str(&tlsp, u"-", TOK_OP))
        *numsign = -1;
    else if(expect_str(&tlsp, u"-", TOK_OP))
        *numsign = +1;
    if(expect(&tlsp, num, TOK_NUM)) {
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_scale_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *num; int numsign;

    /* ⟨scale statement⟩ ::= ‘scale’ ⟨NUM⟩ */
    if(expect_str(&tlsp, u"scale", TOK_ID)
    && parse_constexpr(&tlsp, &numsign, &num)) /* a string value */
    {
        add_code_real(the_code, numsign, num);
        add_code_str(the_code, ":ImgSetScale");
        add_code_NL(the_code);
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_projection_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *num1, *num2, *num3, *num4;
    int sign1, sign2, sign3, sign4;

    /* ⟨size statement⟩ ::= ‘size’ ⟨NUM⟩ ⟨NUM⟩ */
    if(expect_str(&tlsp, u"projection", TOK_ID)
    && expect_str(&tlsp, u"Lambert", TOK_ID) /* QnD coding, for now */
    && parse_constexpr(&tlsp, &sign1, &num1)  /* a string value */
    && parse_constexpr(&tlsp, &sign2, &num2)  /* a string value */
    && parse_constexpr(&tlsp, &sign3, &num3)  /* a string value */
    && parse_constexpr(&tlsp, &sign4, &num4)) /* a string value */
    {
        add_code_real(the_code, sign1, num1); add_code_NL(the_code);
        add_code_real(the_code, sign2, num2); add_code_NL(the_code);
        add_code_real(the_code, sign3, num3); add_code_NL(the_code);
        add_code_real(the_code, sign4, num4); 
        add_code_str(the_code, ":ImgSetLambert");
        add_code_NL(the_code);
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_label_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    /* token *varname; */

    /* ⟨label statement⟩ ::= ⟨KW⟩ ‘:’ */
    if(expect_str(&tlsp, u"image_data", TOK_ID)
    && expect_ch(&tlsp, L':', TOK_OP)) /* a string value */
    {
        the_code->vm_current = VM_IMAGE_DATA_LAYER;
        *TLSP = tlsp;
        return 1;
    }
    else if(expect_str(&tlsp, u"init_drawing", TOK_ID)
    && expect_ch(&tlsp, L':', TOK_OP)) /* a string value */
    {
        the_code->vm_current = VM_INIT_DRAWING_LAYER;
        *TLSP = tlsp;
        return 1;
    }
    else if(expect_str(&tlsp, u"support_drawing", TOK_ID)
    && expect_ch(&tlsp, L':', TOK_OP)) /* a string value */
    {
        the_code->vm_current = VM_SUPPORT_DRAWING_LAYER;
        *TLSP = tlsp;
        return 1;
    }
    else if(expect_str(&tlsp, u"real_objects", TOK_ID)
    && expect_ch(&tlsp, L':', TOK_OP)) /* a string value */
    {
        the_code->vm_current = VM_REAL_OBJECTS_LAYER;
        *TLSP = tlsp;
        return 1;
    }
    else if(expect_str(&tlsp, u"final", TOK_ID)
    && expect_ch(&tlsp, L':', TOK_OP)) /* a string value */
    {
        the_code->vm_current = VM_FINAL_LAYER;
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int OK(char *trc) {
    printf("OK: %s\n", trc);
    return 1;
}

int parse_load_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *dbname, *filename;
    int ix; uchar *db;

    if(expect_str(&tlsp, u"load", TOK_ID)
    && expect(&tlsp, &dbname, TOK_ID)      /* the database name */
    && expect(&tlsp, &filename, TOK_CSTR)) /* the file name */
    {
        ix = add_cstr(the_code, filename->V.cstr);
        add_code_cstr_ix(the_code, ix);
        db = tok_ustr(dbname);
        if(0 == ucscmp(db, u"bounds"))
            add_code_str(the_code, ":LoadBounds");
        else if(0 == ucscmp(db, u"labels"))
            add_code_str(the_code, ":LoadLabels");
        else if(0 == ucscmp(db, u"lines"))
            add_code_str(the_code, ":LoadLines");
        else if(0 == ucscmp(db, u"stars"))
            add_code_str(the_code, ":LoadStars");
        add_code_NL(the_code);
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_open_file_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *filename;
    int ix;
    char *fname, buf[1024];

    if(expect_str(&tlsp, u"open", TOK_ID)
    && expect_str(&tlsp, u"file", TOK_ID)   /* the database name */
    && expect(&tlsp, &filename, TOK_CSTR))  /* the file name */
    {
        ix = add_cstr(the_code, filename->V.cstr);
        add_code_cstr_ix(the_code, ix);
        fname = tok_str(buf, filename, 1024);
        add_code_str(the_code, ":OpenFile");
        add_code_NL(the_code);
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_draw_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    token *dbname, *groupname;
    int ix; uchar *db, *group;

    if(expect_str(&tlsp, u"draw", TOK_ID)
    && expect(&tlsp, &dbname, TOK_ID))  /* the file name */
    {
        *TLSP = tlsp;
        db = tok_ustr(dbname);
        if(0 == ucscmp(db, u"background"))
            add_code_str(the_code, ":DrawBackground");
        else if(0 == ucscmp(db, u"bounds"))
            add_code_str(the_code, ":DrawBounds");
        else if(0 == ucscmp(db, u"debug_info"))
            add_code_str(the_code, ":DrawDebugInfo");
        else if(0 == ucscmp(db, u"footer"))
            add_code_str(the_code, ":DrawFoot");
        else if(0 == ucscmp(db, u"grid"))
            add_code_str(the_code, ":DrawGrid");
        else if(0 == ucscmp(db, u"head"))
            add_code_str(the_code, ":DrawHead");
        else if(0 == ucscmp(db, u"stars"))
            add_code_str(the_code, ":DrawStars");
        else {
            /* select subclause is optional: */
            if(expect_str(&tlsp, u"select", TOK_ID)
            && expect(&tlsp, &groupname, TOK_USTR))
            {
                ix = add_ustr(the_code, tok_ustr(groupname));
                group = tok_ustr(groupname);
                add_code_ustr_ix(the_code, ix);
                if(0 == ucscmp(db, u"delportian"))
                    add_code_str(the_code, ":DrawDelportianArea");
                else if(0 == ucscmp(db, u"labels"))
                    add_code_str(the_code, ":DrawLabels");
                else if(0 == ucscmp(db, u"lines"))
                    add_code_str(the_code, ":DrawLines");
                add_code_NL(the_code);
                *TLSP = tlsp;
                return 1;
            }
            return 0;
        }
        add_code_NL(the_code);
        return 1;
    }
    return 0;
}

int parse_close_stmt(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;

    if(expect_str(&tlsp, u"close", TOK_ID)
    && expect_str(&tlsp, u"file", TOK_ID))
    {
        *TLSP = tlsp;
        return 1;
    }
    return 0;
}

int parse_image_stmt_seq(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;
    int label;

    add_code_str(the_code, ":NewImage");
    add_code_NL(the_code);

    /* ( ⟨set statement⟩ ) [ ‘;’ ⟨image sequence⟩ ] */
    /** add more image statement types!! */
    while(1) {
        label = 0;
        if(parse_name_stmt(&tlsp)) {
            printf("name\n");
            *TLSP = tlsp;
        }
        else if(parse_size_stmt(&tlsp)) {
            printf("size\n");
            *TLSP = tlsp;
        }
        else if(parse_scale_stmt(&tlsp)) {
            printf("scale\n");
            *TLSP = tlsp;
        }
        else if(parse_projection_stmt(&tlsp)) {
            printf("projection\n");
            *TLSP = tlsp;
        }
        else if(parse_label_stmt(&tlsp)) {
            printf("label\n");
            label = 1; /* QnD progm, not proper parsing!! */
            *TLSP = tlsp;
        }
        else if(parse_load_stmt(&tlsp)) {
            printf("load\n");
            *TLSP = tlsp;
        }
        else if(parse_open_file_stmt(&tlsp)) {
            printf("open\n");
            *TLSP = tlsp;
        }
        else if(parse_draw_stmt(&tlsp)) {
            printf("draw\n");
            *TLSP = tlsp;
        }
        else if(parse_close_stmt(&tlsp)) {
            printf("close\n");
            *TLSP = tlsp;
        }
        /* use 'else if' branches to detect alternative statements here */

        if(!label) { /* QnD prog, not proper parsing!! */
            if(expect_ch(&tlsp, L';', TOK_OP)) {
                *TLSP = tlsp;
            }
            else {
                return 1;
            }
        }
        else {
            *TLSP = tlsp;
        }
    }
    printf("Parse error: statement sequence\n");
    return 0;
}

int parse_image(token_liststream **TLSP) {
    token_liststream *tlsp = *TLSP;

    /* ‘image’ ⟨name⟩ ‘is’ ⟨name of inherited image⟩ ‘{’ ⟨image sequence⟩ ‘}’ */
    /** also allow a variant with no image inheritance */
    if(expect_str(&tlsp, u"image", TOK_ID)
    && expect_ch(&tlsp, L'{', TOK_LPAR)
    && parse_image_stmt_seq(&tlsp)
    && expect_ch(&tlsp, L'}', TOK_RPAR))
    {
        *TLSP = tlsp;
        return 1;
    }
    printf("Parse error: malformed image definition\n");
    dump_token_liststream(tlsp);

    return 0;
}

int parse_program(token_file *TF) {
    token_liststream *TLS = new_token_liststream(TF), *tls;

    tls = TLS;

    /* should 1. parse version, 2. depending on version
       choose parser */
    if(parse_version(&tls) && parse_image(&tls)) {
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
    the_code = new_code();
    output_file = fopen(new_ext(argv[1],"mbf"), "wt");
    /*printf("INFO: output file '%s'\n", new_ext(argv[1],"mbf"));*/

    if (!parse_program(TF))
        printf("PARSE FAILED\n");
    else
        dump_code(the_code);

    tokfclose(TF);
    fclose(output_file);

    return 0;
}
