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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meta.h"
#include "defs.h"
#include "ucstr.h"
#include "uctype.h"
#include "usio.h"
#include "token.h"

token_file *tokfopen(char *fname) {
    token_file *res = ALLOC(token_file);
    res->tok_file = u8fopen(fname, "rt");
    if(!res->tok_file) { free(res); return 0; }
    res->tok_save = 0;        /* init with NO token lookahead */
    return res;
}

int tokfclose(token_file *tok_stream) {
    utf8_file *tok_file = tok_stream->tok_file;
    free(tok_stream);
    return u8fclose(tok_file);
}

token *_new_token_raw(token_type type, int line_num) {
    token *res = ALLOC(token);
    res->type = type;
    res->unit = 0;
    res->line_num = line_num;
    return res;
}

token *_new_token(token_type type, uchar *ustr, int line_num) {
    token *res = _new_token_raw(type, line_num);
    res->V.ustr = ucsdup(ustr);
    return res;
}

token *_new_token_cstr(char *str, int line_num) {
    token *res = _new_token_raw(TOK_CSTR, line_num);
    res->V.cstr = strdup(str);
    return res;
}

token *_new_token_num(token_type type, uchar *ustr, uchar *unit, base_mode base, int line_num) {
    token *res = _new_token_raw(type, line_num);
    res->V.ustr = ucsdup(ustr);
    res->unit = ucsdup(unit);
    res->base = base;
    return res;
}

token *_new_token_uchar(token_type type, uchar uc, int line_num) {
    token *res = _new_token_raw(type, line_num);
    res->V.ustr = ALLOCN(uchar,2);
    res->V.ustr[0] = uc; res->V.ustr[1] = L'\0';
    return res;
}

int ishms(uch) {
    return uch == L'ʰ' || uch == L'ᵐ' || uch == L'ˢ';
}

int isdms(uch) {
    return uch == L'°' || uch == L'\'' || uch == L'"';
}

base_mode scan_mode(uchar *ustr) {
    base_mode bmode;
    int ix;

    bmode = BAS_DEC;
    for (ix = 0; ustr[ix]; ix++) {
        switch (ustr[ix]) {
            case L'ʰ':
                if (bmode == BAS_DEC) bmode = BAS_H; else return BAS_ERROR;
                ustr[ix] = L':';
                break;
            case L'°':
                if (bmode == BAS_DEC) bmode = BAS_D; else return BAS_ERROR;
                ustr[ix] = L':';
                break;
            case L'ᵐ':
                if (bmode == BAS_H) bmode = BAS_HM; else return BAS_ERROR;
                ustr[ix] = L':';
                break;
            case L'\'':
                if (bmode == BAS_D) bmode = BAS_DM; else return BAS_ERROR;
                ustr[ix] = L':';
                break;
            case L'ˢ':
                if (bmode == BAS_HM) bmode = BAS_HMS; else return BAS_ERROR;
                ustr[ix] = L':';
                break;
            case L'"':
                if (bmode == BAS_DM) bmode = BAS_DMS; else return BAS_ERROR;
                ustr[ix] = L':';
                break;
            default: break;
        }
    }
    return bmode;
}

token *_scan(utf8_file *stream) {
    uchar ustr[256], uunit[32];
    uchar uch;
    char cstr[256];
    int ix, jx, lno;

    while (isuws(uch = fgetuc(stream)));
    while (uch == L'#') {
        while ((uch = fgetuc(stream)) != L'\n');
        while (isuws(uch = fgetuc(stream)));
    }

    lno = u8flineno(stream);
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
        return _new_token(TOK_USTR, ustr, lno);
    }
    else if (uch == L'"') {
        int slev = 1;

        ix = 0;
        while (slev > 0) {
            uch = fgetuc(stream);
            ustr[ix] = uch;
            ix ++;
            if (uch == L'"') {
                slev++; break;
            }
        }
        ustr[ix-1] = L'\0';
        return _new_token(TOK_USTR, ustr, lno);
    }
    else if (uch == L'(') {
        int slev = 1;
        int nuch;

        nuch = fgetuc(stream);
        if (nuch != L'%') {
            fungetuc(nuch, stream);
            return _new_token_uchar(TOK_LPAR, uch, lno);
        }

        ix = 0;
        while (slev > 0) {
            uch = fgetuc(stream);
            ustr[ix] = uch;
            ix ++;
            if (uch == L'%') {
                nuch = fgetuc(stream);
                if (nuch != L')') {
                    fungetuc(nuch, stream); continue;
                } else {
                    slev++; break;
                }
            }
        }
        ustr[ix-1] = L'\0';
        return _new_token(TOK_VERSION, ustr, lno);
    }
    else if (uch == L'\'') {
        int slev = 1;

        ix = 0;
        while (slev > 0) {
            uch = fgetuc(stream);
            cstr[ix] = (char)uch;
            ix ++;
            if (uch == L'\'') {
                  slev++; break;
            }
        }
        cstr[ix-1] = '\0';
        return _new_token_cstr(cstr, lno);
    }
    else if (uch == L'‘') {
        int slev = 1;

        ix = 0;
        while (slev > 0) {
            uch = fgetuc(stream);
            cstr[ix] = (char)uch;
            ix ++;
            if (uch == L'’') {
                  slev++; break;
            }
        }
        cstr[ix-1] = '\0';
        return _new_token_cstr(cstr, lno);
    }
    else if (isunum(uch)) {
        /* NUMBER WITH SCALE FACTOR AND UNIT */
        base_mode bmode;

        ustr[0] = uch;
        ix = 0;
        while (isunum(uch) || isualpha(uch)
        || ishms(uch) || isdms(uch) || uch == L'.') {
            ix ++; uch = fgetuc(stream); ustr[ix] = uch;
        }
        ustr[ix] = L'\0';
        fungetuc(uch, stream);
        /* from end of ustr find last alphabetic char = start of unit */
        for (ix--; isualpha(ustr[ix]); ix--);
        /* from there copy unit str to uunit */
        for (jx = 0, ix++; ustr[ix]; ix++, jx++) {
            uunit[jx] = ustr[ix];
            if(!jx) ustr[ix] = 0;    /* cut number before unit substr */
        }
        uunit[jx] = L'\0';
        /** Handle scale (ʰᵐˢ vs. °′″) and number format (sex/hex/dec) **/
        bmode = scan_mode(ustr);

        return _new_token_num(TOK_NUM, ustr, uunit, bmode, lno);
    }
    else if (isualpha(uch)) {
        ustr[0] = uch;
        ix = 0;
        while (isualnum(uch)) {
            ix ++;
            uch = fgetuc(stream);
            ustr[ix] = uch;
        }
        fungetuc(uch, stream);
        ustr[ix] = L'\0';
        return _new_token(TOK_ID, ustr, lno);
    }
    else if (!isualpha(uch)) {
        token_type T;
        if (uch == L'{' || uch == L'[' || uch == L'(') {
            T = TOK_LPAR;
        }
        else if (uch == L'}' || uch == L']' || uch == L')') {
            T = TOK_RPAR;
        }
        else if (uch == -1) {
            return _new_token(TOK_NONE, 0, lno);
        }
        else {
            T = TOK_OP;
        }
        return _new_token_uchar(T, uch, lno);
    }
    return _new_token(TOK_NONE, 0, lno);
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

int is_type(token *tok, token_type type) {
    if (tok->type != type) return 0;
    return 1;
}

int is_str(token *tok, uchar *op) {
    if (0 != ucscmp(tok->V.ustr, op)) return 0;
    return 1;
}

int is_item(token *tok, uchar *op, token_type type) {
    return is_str(tok, op) && is_type(tok, type);
}

int is_num(token *tok) { return is_type(tok, TOK_NUM); }
int is_any_kw(token *tok) { return is_type(tok, TOK_ID); }
int is_none(token *tok) { return is_type(tok, TOK_NONE); }

int is_op(token *tok, uchar *op)   { return is_item(tok, op, TOK_OP);   }
int is_kw(token *tok, uchar *op)   { return is_item(tok, op, TOK_ID);   }
int is_lpar(token *tok, uchar *op) { return is_item(tok, op, TOK_LPAR); }
int is_rpar(token *tok, uchar *op) { return is_item(tok, op, TOK_RPAR); }

char *type_str(token_type type) {
    switch(type) {
        case TOK_NONE: return "ERR";
        case TOK_ID: return "IDENT";
        case TOK_OP: return "OP";
        case TOK_NUM: return "NUM";

        case TOK_VERSION: return "VERSION";

        case TOK_CSTR: return "CSTR";
        case TOK_USTR: return "USTR";

        case TOK_LPAR: return "LPAR";
        case TOK_RPAR: return "RPAR";
        default: return "unknown";
    }
    return "unknown";
}

char *tok_type_str(token *tok) {
    return type_str(tok->type);
    /*switch(tok->type) {
        case TOK_NONE: return "ERR";
        case TOK_ID: return "IDENT";
        case TOK_OP: return "OP";
        case TOK_NUM: return "NUM";

        case TOK_VERSION: return "VERSION";

        case TOK_CSTR: return "CSTR";
        case TOK_USTR: return "USTR";

        case TOK_LPAR: return "LPAR";
        case TOK_RPAR: return "RPAR";
        default: return "unknown";
    }
    return "unknown";*/
}

uchar empty[] = {0};

uchar *tok_ustr(token *tok) {
    if (tok->type == TOK_CSTR) return empty;
    return tok->V.ustr;
}

char *tok_str(char *buf, token *tok, int size) {
    if (tok->type == TOK_CSTR)
        return strncpy(buf, tok->V.cstr, size);
    return ucstombs(buf, tok->V.ustr, size);
}

char *tok_unit(char *buf, token *tok, int size) {
    if (tok->type != TOK_NUM) return "";
    return ucstombs(buf, tok->unit, size);
}

char *tok_base_name(token *tok) {
    switch (tok->base) {
        case BAS_DEC: return "decimal";
        case BAS_HEX: return "hexadecimal";
        case BAS_OCT: return "octal";
        case BAS_H: return "h";
        case BAS_HM: return "hm";
        case BAS_HMS: return "hms";
        case BAS_D: return "d";
        case BAS_DM: return "dm";
        case BAS_DMS: return "dms";
        case BAS_ERROR: return "error";
        default: return "unknown";
    }
    return 0;
}

int tok_int(token *tok) {
    char buf[1024];
    /** INTE BRA KOD, LÄGG IN TESTER BASERADE PÅ token_type/base_mode */
    tok_str(buf, tok, 1024);
    return atoi(buf);
}

double tok_real(token *tok) {
    char buf[1024];
    /** INTE BRA KOD, LÄGG IN TESTER BASERADE PÅ token_type/base_mode */
    tok_str(buf, tok, 1024);
    return atof(buf);
}

void tok_free(token *tok) {
    if (tok->type == TOK_CSTR)
        free(tok->V.cstr);
    else
        free(tok->V.ustr);
    free(tok->unit);
    free(tok);
}
