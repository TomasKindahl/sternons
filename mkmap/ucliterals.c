#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "meta.h"
#include "usio.h"
#include "ucstr.h"

typedef struct _ucs_S {
    uchar *str;
    int pos;
} ucs;

typedef struct _ustring_S {
    struct _ucs_S cont;
    struct _ustring_S *next;
} ustring;

ustring *new_ustring(ustring *next) {
    ustring *res = ALLOC(ustring);
    res->cont.str = ALLOCN(uchar,256);
    res->cont.pos = 0;
    res->next = next;
    return res;
}

ustring *new_ustring_ustr(uchar *ustr, ustring *next) {
    ustring *res = new_ustring(next);
    ucscpy(res->cont.str, ustr);
    res->cont.pos = ucslen(ustr);
    return res;
}

void add_uch_ustring(ustring *ustr, int uch) {
    ustr->cont.str[ustr->cont.pos++] = uch;
}

char *encode(uchar ch);

int num_ustring(ustring *ustr) {
    if(!ustr) return 0;
    if(!ustr->cont.str[0]) return num_ustring(ustr->next);
    return 1 + num_ustring(ustr->next);
}

void set_ucs(ucs *arr, ustring *ustr) {
    ustring *uptr = ustr;
    int ix;

    for (ix = 0; uptr; uptr = uptr->next) {
        if (uptr->cont.str[0])
            arr[ix++] = uptr->cont;
    }
    return;
}

void sort_ucs(ucs *arr, int len) {
    int ix, jx;
    /* Bubble sort */
    for(ix = 0; ix < len; ix++) {
        for(jx = ix+1; jx < len; jx++) {
            if(0 < ucscmp(arr[ix].str, arr[jx].str)) {
                ucs tmp = arr[ix];
                arr[ix] = arr[jx];
                arr[jx] = tmp;
            }
        }
    }
}

void rm_dupls(ucs *arr, int len) {
    int ix, jx;
    for(ix = 0; ix < len; ix++) {
        if(!arr[ix].str[0]) continue;
        for(jx = ix+1; jx < len; jx++) {
            if(0 == ucscmp(arr[ix].str, arr[jx].str)) {
                printf("%i ~ %i\n", ix, jx);
                arr[jx].str[0] = 0;
                arr[jx].pos = 0;
            }
        }
    }
}

void write_ucs(FILE *outp, ucs ustr) {
    char buf[256];
    int ix;
    if(!ustr.str[0]) return;
    fprintf(outp, "uchar _UC_");
    for (ix = 0; ix < ustr.pos; ix++) {
        uchar ch = ustr.str[ix];
        if(ch != 0)
            fprintf(outp, "%s", encode(ch));
    }
    fprintf(outp, "[] = { ");
    for (ix = 0; ix < ustr.pos; ix++) {
        uchar ch = ustr.str[ix];
        if (ch == 0) 
            fprintf(outp, "0 };");
        else if (' ' <= ch && ch <= '~')
            fprintf(outp, "'%c', ", (char)ch);
        else 
            fprintf(outp, "0x%X, ", ch);
    }
    fprintf(outp, " /* %s */\n", ucstombs(buf, ustr.str, 256));
}

void dump_ucsarr(FILE *outp, ucs *ustr, int size) {
    int ix;
    for (ix = 0; ix < size; ix++) {
        write_ucs(outp, ustr[ix]);
    }
}

ustring *stringlist = 0;

char *encode(uchar ch) {
    static char str[20];

    if('A' <= ch && ch <= 'Z') {
        sprintf(str,"%c",(char)ch);
        return str;
    }
    if('a' <= ch && ch <= 'z') {
        sprintf(str,"%c",(char)ch);
        return str;
    }
    sprintf(str,"_%X_",ch);
    return str;
}

void ucliterals(char *fname) {
    utf8_file *inf = u8fopen(fname,"rt");
    uchar ch0, ch;
    char buf[128];

    if (!inf) return;
    ch0 = 0;
    while((ch = fgetuc(inf)) != -1) {
        if (ch0 == L'"') {
            if (ch == L'"') {
                add_uch_ustring(stringlist, 0);
                stringlist = new_ustring(stringlist);
                ch0 = 0;
            }
            else {
                add_uch_ustring(stringlist, ch);
                printf("%s", encode(ch));
            }
        }
        else {
            switch(ch) {
                case L'u':
                    ch0 = L'u';
                    break;
                case L'"':
                    if (ch0 == L'u') {
                        ch0 = L'"';
                        printf("_UC_");
                    }
                    else 
                        printf("%s", uctombs(buf,ch));
                    break;
                default:
                    if (ch0 == L'u') {
                        ch0 = 0;
                        printf("u");
                    }
                    printf("%s", uctombs(buf,ch));
                    break;
            }
        }
    }
    u8fclose(inf);
}

int hex(uchar val) {
    if('0'<=val && val <='9') return val-'0';
    if('A'<=val && val <='F') return val-'A'+10;
    if('a'<=val && val <='f') return val-'a'+10;
    return 0;
}

void read_all_strings(ustring *UL) {
    FILE *all_strings_h = fopen("allstrings.h","rt");
    char line[1024], buf[256];
    uchar ustr[256];
    int lix, uix, val;

    if(!all_strings_h) return;

    while(fgets(line, 1023, all_strings_h)) {
        if(0 == strncmp(line,"uchar _UC_",10)) {
            uix = 0;
            for(lix = 10; line[lix] != '['; lix++) {
                if(line[lix] == '_') {
                    val = 0;
                    while(line[++lix] != '_') {
                       val = (val<<4)+hex(line[lix]);
                    }
                    ustr[uix++] = val;
                }
                else
                    ustr[uix++] = line[lix];
            }
            ustr[uix] = 0;
            printf("'%s'\n", ucstombs(buf,ustr,256));
            stringlist = new_ustring_ustr(ustr, stringlist);
        }
    }

    fclose(all_strings_h);
}

void write_all_strings(ustring *UL) {
    ucs *arr;
    int len;
    FILE *all_strings_h = fopen("allstrings.h","wt"); /* OVERWRITE! */

    if(!all_strings_h) {
        fprintf(stderr,"FATAL: couldn't open file allstrings.h\n");
        exit(-1);
    }
    fprintf(all_strings_h,"/* WARNING: autogenerated file - DON'T EDIT! */\n");
    fprintf(all_strings_h,"#ifndef _ALLSTRINGS_H\n");
    fprintf(all_strings_h,"#define _ALLSTRINGS_H\n\n");

    len = num_ustring(stringlist);
    fprintf(stderr, "num elements: %i\n", len);
    arr = ALLOCN(ucs, len);
    set_ucs(arr, stringlist);
    sort_ucs(arr, len);
    rm_dupls(arr, len);
    dump_ucsarr(all_strings_h, arr, len);

    fprintf(all_strings_h,"\n#endif /* _ALLSTRINGS_H */\n\n");
    fclose(all_strings_h);
}

int main (int argc, char **argv) {
    int aix;
    printf("/* DEMO: this is to be dumped in the .h file */\n");

    stringlist = new_ustring(0);

    for (aix = 1; aix < argc; aix++) {
        ucliterals(argv[aix]);
        /* printf("%i: %s\n", aix, argv[aix]); */
    }
    read_all_strings(stringlist);
    write_all_strings(stringlist);
    return 0;
}

