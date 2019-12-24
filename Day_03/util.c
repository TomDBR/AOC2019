#include "util.h"
#include <stdio.h>

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

