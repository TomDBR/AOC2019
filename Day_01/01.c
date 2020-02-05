#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	char *input = "./01_input.txt";
	FILE *ifp;
	char curLine[20];
	int total = 0;
	ifp = fopen(input, "r");
	if (ifp == NULL) {
		fprintf(stderr, "can't open input\n");
		exit(1);
	}
	while(1) {
		int num = atoi(fgets(curLine, 20, ifp));
		int fuel = ( num / 3 ) - 2;
		total += fuel;
		while(fuel >= 0) {
			fuel = ( fuel / 3 ) - 2;
			if (fuel > 0) total += fuel;
		}
		if(feof(ifp)) break;
	}
	fclose(ifp);
	printf("total is: %d\n", total);
	return 0;
}
