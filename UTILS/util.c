#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int abs (int a) {
	return a <= 0 ? -a : a;
}

int between (int a, int one, int two) {
	if ((one < a && a < two) || (two < a && a < one)) {
		return 1;
	}
	return 0;
}

int getAmountOfChars(int input)
{
	char tmpstr[10];
	sprintf(tmpstr, "%d", input);
	char *ptr = tmpstr;
	int count = 0;
	while (*ptr) {
		if(*ptr != '\0') count++;
		ptr++;
	}
	return count;
}

void die(char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int receiveNumber() 
{
	int number = -1;
	int retVal = 0 ;
	while(retVal != 1) {
		retVal = fscanf(stdin, "%d", &number);
		if (retVal == 0) {
			fprintf(stderr, "Input needs to be numeric!\n");
			while(fgetc(stdin) != '\n');
		}
	}
	return number;
}
