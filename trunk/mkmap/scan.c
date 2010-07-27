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
	res->tok_save = 0;		/* init with NO token lookahead */
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
	if (type == TOK_NUM) res->num = ucstof(ucs);
	return res;
}

token *_scan(utf8_file *stream) {
	uchar ustr[1024];
	uchar uch;
	int ix;

	while (isuws(uch = fgetuc(stream)));
	if (uch == L'“') {
		ix = 0;
		while (uch != L'”') {
			uch = fgetuc(stream);
			ustr[ix] = uch;
			ix ++;
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
		fungetuc(uch, stream);
		ustr[ix] = L'\0';
		return _new_token(TOK_NUM, ustr);
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
		if (ures[0] == L'{' || ures[0] == L'[')
			T = TOK_LPAR;
		else if (ures[0] == L'}' || ures[0] == L']')
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

int is_item(token *tok, uchar *op, token_type type) {
	if (tok->type != type) return 0;
	if (0 != ucscmp(tok->ucs, op)) return 0;
	return 1;
}

int is_op(token *tok, uchar *op) { 	return is_item(tok, op, TOK_OP); }
int is_kw(token *tok, uchar *op) { 	return is_item(tok, op, TOK_KW); }
int is_lpar(token *tok, uchar *op) { return is_item(tok, op, TOK_LPAR); }
int is_rpar(token *tok, uchar *op) { return is_item(tok, op, TOK_RPAR); }
int is_num(token *tok) { return tok->type == TOK_NUM; }

char *tok_name(int type) {
	switch(type) {
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

