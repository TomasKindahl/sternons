#ifndef _SCAN_H
#define _SCAN_H

typedef enum {
    TOK_NONE, TOK_STR, TOK_KW, TOK_OP, TOK_LPAR, TOK_RPAR, TOK_NUM
} token_type;

typedef struct _token_S {
    token_type type;
    uchar *ucs;
    double num;
} token;

typedef struct _token_file_S {
    /* specialized token file allowing token get and unget */
    utf8_file *tok_file;
    token *tok_save;
} token_file;

token_file *tokfopen(char *fname);
int tokfclose(token_file *tok_stream);
token *scan(token_file *tok_stream);
int unscan(token *value, token_file *tok_stream);

int is_item(token *tok, uchar *op, token_type type);
int is_op(token *tok, uchar *op);
int is_kw(token *tok, uchar *op);
int is_lpar(token *tok, uchar *op);
int is_rpar(token *tok, uchar *op);
int is_num(token *tok);
char *tok_name(int type);

#endif /* _SCAN_H */