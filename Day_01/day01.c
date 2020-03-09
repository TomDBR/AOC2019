#include <stdio.h>
#include <stdlib.h>
#include "../UTILS/util.h"

int main(int argc, char* argv[]) {
	FILE *ifp;
	int total = 0, num;

	if (!(ifp = fopen("./01_input.txt", "r"))) die("can't open file!");
	while(fscanf(ifp, "%d", &num) == 1) {
		int fuel = ( num / 3 ) - 2;
		total += fuel;
		while(fuel >= 0) {
			fuel = ( fuel / 3 ) - 2;
			if (fuel > 0) total += fuel;
		}
	}
	fclose(ifp);
	printf("total is: %d\n", total);
	return 0;
}
