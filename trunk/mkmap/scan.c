/************************************************************************
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

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "ucstr.h"
#include "uctype.h"
#include "usio.h"
#include "scan.h"

token_file *tokfopen(char *fname) {
    token_file *res = (token_file *)malloc(sizeof(token_file));
    res->tok_file = u8fopen(fname);
    if(!res->tok_file) { free(res); return 0; }
    res->tok_save = 0;        /* init with NO token lookahead */
    return res;
}

int tokfclose(token_file *tok_stream) {
    utf8_file *tok_file = tok_stream->tok_file;
    free(tok_stream);
    return u8fclose(tok_file);
}

token *_new_token(token_type type, uchar *ucs) {
    token *res;
    res = (token *)malloc(sizeof(token));
    res->type = type;
    res->ucs = ucsdup(ucs);
    res->unit = 0;
    if (type == TOK_NUM) res->num = ucstof(ucs);
    return res;
}

token *_new_token_num(token_type type, uchar *ucs, uchar *unit) {
    token *res;
    res = (token *)malloc(sizeof(token));
    res->type = type;
    res->ucs = ucsdup(ucs);
    res->unit = ucsdup(unit);
    res->num = ucstof(ucs);
    return res;
}


token *_scan(utf8_file *stream) {
    uchar ustr[256], uunit[256];
    uchar uch;
    int ix, jx;

    while (isuws(uch = fgetuc(stream)));
    if (uch == L'“') {
    	int slev = 1;
        ix = 0;
        while (slev > 0) {
            uch = fgetuc(stream);
            ustr[ix] = uch;
            ix ++;
            switch (uch) {
              case L'“': slev++; break;
              case L'”': slev--; break;
            }
        }
        ustr[ix-1] = L'\0';
        return _new_token(TOK_STR, ustr);
    }
    else if (isunum(uch) || uch == L'-') {
        ustr[0] = uch;
        ix = 0;
        if (uch == L'-') {
            ix ++;
            uch = fgetuc(stream);
            ustr[ix] = uch;
        }
        while (isunum(uch)) {
            ix ++;
            uch = fgetuc(stream);
            ustr[ix] = uch;
        }
        if (uch == L'.') {
            ix ++;
            uch = fgetuc(stream);
            ustr[ix] = uch;
        }
        while (isunum(uch)) {
            ix ++;
            uch = fgetuc(stream);
            ustr[ix] = uch;
        }
        /*! insert exp handling here! */
        /* unit handling: */
       	jx = 0;
        if (isualpha(uch)) {
        	while (isualpha(uch)) {
        		uunit[jx] = uch;
        		uch = fgetuc(stream);
        		jx++;
        	}
        }
        else if (uch == L'°') {
        	uunit[jx] = uch;
       		uch = fgetuc(stream);
       		jx++;
        }
       	uunit[jx] = L'\0';
        fungetuc(uch, stream);
        ustr[ix] = L'\0';
        return _new_token_num(TOK_NUM, ustr, uunit);
    }
    else if (isualpha(uch)) {
        ustr[0] = uch;
        ix = 0;
        while (isualpha(uch)) {
            ix ++;
            uch = fgetuc(stream);
            ustr[ix] = uch;
        }
        fungetuc(uch, stream);
        ustr[ix] = L'\0';
        return _new_token(TOK_KW, ustr);
    }
    else if (!isualpha(uch)) {
        uchar *ures = (uchar *)malloc(sizeof(uchar)*2);
        token_type T;
        ures[0] = uch;
        ures[1] = L'\0';
        if (ures[0] == L'{' || ures[0] == L'[' || ures[0] == L'(')
            T = TOK_LPAR;
        else if (ures[0] == L'}' || ures[0] == L']' || ures[0] == L')')
            T = TOK_RPAR;
        else
            T = TOK_OP;
        return _new_token(T, ures);
    }
    return _new_token(TOK_NONE, 0);
}

token *scan(token_file *tok_stream) {
    if (tok_stream->tok_save != 0) {
        token *res = tok_stream->tok_save;
        tok_stream->tok_save = 0;
        return res;
    }
    return _scan(tok_stream->tok_file);
}

int unscan(token *value, token_file *tok_stream) {
    if (tok_stream->tok_save != 0) return 0;
    tok_stream->tok_save = value;
    return 1;
}

int tokfeof(token_file *tok_stream) {
	return u8feof(tok_stream->tok_file) && tok_stream->tok_save == 0;
}

int is_item(token *tok, uchar *op, token_type type) {
    if (tok->type != type) return 0;
    if (0 != ucscmp(tok->ucs, op)) return 0;
    return 1;
}

int is_op(token *tok, uchar *op) {     return is_item(tok, op, TOK_OP); }
int is_kw(token *tok, uchar *op) {     return is_item(tok, op, TOK_KW); }
int is_lpar(token *tok, uchar *op) { return is_item(tok, op, TOK_LPAR); }
int is_rpar(token *tok, uchar *op) { return is_item(tok, op, TOK_RPAR); }
int is_num(token *tok) { return tok->type == TOK_NUM; }

char *tok_type_str(token *tok) {
    switch(tok->type) {
        case TOK_NONE:
            return "ERR";
        case TOK_STR:
            return "STR";
        case TOK_KW:
            return "KW";
        case TOK_OP:
            return "OP";
        case TOK_LPAR:
            return "LPAR";
        case TOK_RPAR:
            return "RPAR";
        case TOK_NUM:
            return "NUM";
        default:
            return "unknown";
    }
    return "unknown";
}

uchar *tok_ustr(token *tok) {
	return tok->ucs;
}

char *tok_str(char *buf, token *tok, int size) {
	return ucstombs(buf, tok->ucs, size);
}

char *tok_unit(char *buf, token *tok, int size) {
	if (tok->type != TOK_NUM) return "";
	return ucstombs(buf, tok->unit, size);
}

void tok_free(token *tok) {
	free(tok->ucs);
	free(tok);
}
