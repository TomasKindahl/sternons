#ifndef _UCSTR_H
#define _UCSTR_H

int ucslen(uchar *ustr);
uchar *ucscpy(uchar *dest, uchar *src);
int ucscmp(uchar *ucs1, uchar *ucs2);
uchar *ucschr(uchar *haystack, uchar needle);
uchar *ucsdup(uchar *src);
long ucstoi(uchar *ucs);
double ucstof(uchar *ucs);

#endif /* _UCSTR_H */