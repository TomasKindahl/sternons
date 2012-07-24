#define _ISOC99_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long long to_long_long(double D) {
	union {
		double D;
		long long LL;
	} res;
	res.D = D;
	return res.LL;
}

double to_double(long long LL) {
	union {
		double D;
		long long LL;
	} res;
	res.LL = LL;
	return res.D;
}

int main(int argc, char **argv) {
	printf("double = %ib\n", sizeof(double));
	printf("int = %ib\n", sizeof(int));
	printf("long int = %ib\n", sizeof(long int));
	printf("long long int = %ib\n", sizeof(long long int));
	printf("int* = %ib\n", sizeof(int*));
	if(argc != 3) {
		printf("usage: formats /type/ /value/\n");
		return -1;
	}
	if(0==strcmp(argv[1],"int")) {
		printf("INT %i\n",atoi(argv[2]));
	}
	else if(0==strcmp(argv[1],"real")) {
		int E, S;
		long long M0, M;
		double R2;
		double R = atof(argv[2]);
		M0 = to_long_long(R);
		M = M0 & 0xFFFFFFFFFFFFF;
		E = (M0 >> 52) & 2047;
		S = M0 >> 63;
		printf("REAL %f\n",R);
		printf("REAL %llx\n", M0);
		printf("REAL %x %x %llx\n", S, E, M);
		M |= 0x4000000000000000;
		R2 = to_double(M);
		printf("REAL %i * %f * 2 ** %i\n", ((S<<1)+1), R2, E-1024);
		printf("REAL %f\n", ((S<<1)+1)*(double)(R2)*exp2(E-1024));
	}
	else {
		printf("ERROR: unknown type '%s'\n", argv[1]);
		return -1;
	}
    return 0;
}
