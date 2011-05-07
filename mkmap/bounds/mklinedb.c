#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct _line_S {
	char con[4];
	double ra, de;
	struct _line_S *next;
} line;

line *new_line(char *con, double ra, double de, line *prev) {
	line *res = (line *)malloc(sizeof(line));
	strcpy(res->con, con);
	res->ra = ra; res->de = de;
	res->next = 0;
	if (prev) prev->next = res;
	return res;
}

void dump_line(line *L) {
	line *LN;
	if (!L || !L->next) return;
	LN = L->next;
	printf("1|%s|%f|%f|%f|%f\n", L->con, L->ra, L->de, LN->ra, LN->de);
	dump_line(L->next);
}

void dump_lines(line *LF, line *LL) {
	if(!LF || !LL) return;
	printf("1|%s|%f|%f|%f|%f\n", LF->con, LL->ra, LL->de, LF->ra, LF->de);
	dump_line(LF);
}

int main(int argc, char **argv) {
	char cline[1024], con[4], pcon[4], typ[2];
	int lno = 1, ix;
	double ra, de;
	line *first, *last;

	strcpy(pcon,"");
	first = 0;

	while (fgets(cline, 1023, stdin)) {
		cline[strlen(cline)-1] = '\0';
		/*printf("% 6i: %s\n", lno++, cline);*/
		sscanf(cline, "%lf %lf %s %s", &ra, &de, con, typ);
		for (ix = 1; con[ix]; ix++) con[ix] = tolower(con[ix]);
		/*printf("  ==> %i|%s Brd|%f|%f\n", 1, con, ra, de);*/
		if (0 != strcmp(pcon, con)) {
			dump_lines(first, last);
			first = 0;
			strcpy(pcon, con);
		}
		if (!first) {
			first = new_line(con, ra, de, 0);
			last = first;
		}
		else {
			last->next = new_line(con, ra, de, last);
			last = last->next;
		}
		/* Add point in point list */
	}
	dump_lines(first, last);
	return 0;
}
