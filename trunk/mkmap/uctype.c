#include "defs.h"
#include "uctype.h"

/* uchar is White Space? */
int isuws(uchar uch) {
    return (uch == L' ') || (uch == L'\t') || (uch == L'\n');
}

    /* uchar is alphabetic char? (only Latin considered yet) */
int isualpha(uchar uch) {
    if (L'_' == uch) return 1;
    if (L'A' <= uch && uch <= L'Z') return 1;
    if (L'a' <= uch && uch <= L'z') return 1;
    if (L'À' <= uch && uch <= L'ʯ' && uch != L'×' && uch != L'÷') return 1;
    if (L'Ά' <= uch && uch <= L'Ͽ' && uch != L'·') return 1;
    return 0;
}
    /* uchar is digit? (only Latin considered yet) */
int isunum(uchar uch) {
    if (L'0' <= uch && uch <= L'9') return 1;
    return 0;
}

/* uchar string functions (cygwin has a UTF-16 wchar_t, we assume UTF-32 - int) */
