#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hasAdjacent(int num) {
	char array[7];
	char *ptr = array;
        snprintf(array, 7, "%d", num);
	while (*++ptr) if(*ptr == *(ptr-1)) return 1;
        return 0;
}

int arrayContainsChar(char *charArray, char karakter) {
	for(int count=0;*charArray;charArray++,count++) {
		if (*charArray == karakter) return count;
	}
	return -1;
}

int hasExactTwoAdjacent(int num) {
	char array[7];
        snprintf(array, 7, "%d", num);
	char *ptr = array;
	int occurences[6];
	char chars[7] = {'x','x','x','x','x','x','\0'};
	memset(occurences, -1, 6 * sizeof(int));
	for (int cnt=0, tmp=0; *ptr; ptr++,cnt++) {
		if ((tmp = arrayContainsChar(chars, *ptr)) != -1) occurences[tmp]++;
		else {
			chars[cnt]=*ptr;
			occurences[cnt]=1;
		}
	}
	for(int x = 0; x < 6; x++) {
		if (occurences[x] == 2) return 1;
	}
        return 0;
}

int doesNotDecrease(int num) {
	char array[7];
	char *ptr = array;
        snprintf(array, 7, "%d", num);
        while (*++ptr) {
		if (*ptr < *(ptr-1)) return 0;
        }
        return 1;
}

int main(int argc, char* argv[]) {
	int bound1 = 248345, bound2 = 746315, count1 = 0, count2 = 0;

	for(int i = bound1; i<=bound2; i++) {
		if(hasAdjacent(i) && doesNotDecrease(i))
			count1++;
	}
	printf("part 1:\t %d matches.\n", count1);

	for(int i = bound1; i<=bound2; i++) {
		if(hasExactTwoAdjacent(i) && doesNotDecrease(i))
			count2++;
	}
	printf("part 2:\t %d matches.\n", count2);

	return 0;
}
