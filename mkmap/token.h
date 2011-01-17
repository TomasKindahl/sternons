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

#ifndef _SCAN_H
#define _SCAN_H

typedef enum {
    TOK_NONE, TOK_STR, TOK_KW, TOK_OP, TOK_LPAR, TOK_RPAR, TOK_NUM
} token_type;

typedef enum {
    BAS_DEC, BAS_HEX, BAS_OCT,
    BAS_H, BAS_HM, BAS_HMS, BAS_D, BAS_DM, BAS_DMS, BAS_ERROR
} base_mode;

typedef struct _token_S {
    token_type type;
    uchar *ustr;
    int line_num;
    /* for numbers only: */
    uchar *unit;
    base_mode base;
} token;

typedef struct _token_file_S {
    /* specialized token file allowing token get and unget */
    utf8_file *tok_file;
    token *tok_save;
} token_file;

/** Stream methods **/
token_file *tokfopen(char *fname);                    /* open token stream   */
int tokfclose(token_file *tok_stream);                /* close token stream  */
token *scan(token_file *tok_stream);                  /* scan a token        */
int unscan(token *value, token_file *tok_stream);     /* unscan it           */
int tokfeof(token_file *tok_stream);                  /* detect if as at end */

/** Type checking methods **/
int is_type(token *tok, token_type type);             /* item has spec type? */
int is_str(token *tok, uchar *op);                    /* any item w expected */
                                                       /* string?             */
int is_num(token *tok);                               /* any number?         */
int is_any_kw(token *tok);                            /* any keyword?        */

/** Type and content checking methods **/
int is_item(token *tok, uchar *op, token_type type);  /* item w expected     */
                                                       /* string and type?    */
int is_op(token *tok, uchar *op);                     /* operator w exp str? */
int is_kw(token *tok, uchar *op);                     /* keyword w exp str?  */
int is_lpar(token *tok, uchar *op);                   /* left paren w str?   */
int is_rpar(token *tok, uchar *op);                   /* or a right?         */

char *tok_type_str(token *tok);
uchar *tok_ustr(token *tok);
char *tok_str(char *buf, token *tok, int size);
char *tok_unit(char *buf, token *tok, int size);
char *tok_base_name(token *tok);
void tok_free(token *tok);

#endif /* _SCAN_H */

