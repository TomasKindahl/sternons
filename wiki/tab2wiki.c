#include <string.h>
#include <stdio.h>

void word (char **pos1, char **pos2) {
	if (strspn(*pos2, "\t\n"))
		while (strspn(*pos2, "\t\n")) (*pos2)++;
	*pos1 = *pos2;
	if (!**pos1) return;
	while (strcspn(*pos2, "\t\n")) (*pos2)++;
	return;
}

int main(int argc, char **argv) {
	char line[1024], *pos1, *pos2;
	char *ip;

	while (fgets(line, 1023, stdin)) {
		int num = 0;
		/*printf("%s",line);*/
		for (pos1 = pos2 = line, num = 0; *pos1; word(&pos1,&pos2), num++) {
			switch(num) {
			  case 0: break;
			  case 1:
			  	printf("||[https://code.google.com/p/");
			  	for (ip = pos1; ip < pos2; ip++) printf("%c",*ip);
			  	printf("/ ");
			  	for (ip = pos1; ip < pos2; ip++) printf("%c",*ip);
			  	printf("]");
			  	break;
			  default:
			  	printf("||");
			  	for (ip = pos1; ip < pos2; ip++) printf("%c",*ip);
			  	break;
			}
		}
		printf("\n");
	}
	return 0;
}