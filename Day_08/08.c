#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	FILE *file = fopen("./08_input.txt", "r");
	if(!file) return 0;

	char fewestZeroes[150];
	char layerDecoded[150];
	char array[15001];
	char *arrPtr = array;
	int lastCount = 150;

	fgets(array, 15001, file);
	fclose(file);

	memset(layerDecoded, '2', 150 * sizeof(char));
	for(int i = 0; i < 100; i++) {
		int cnt = 15000 -((i+1)*150);
		int zeroCount = 0;
		for(int j = 0; j < 150; j++) {
			if (array[cnt+j] == '0') zeroCount++;
			switch (array[cnt+j]) {
				case '0' : // black
					layerDecoded[j] = '0';
					break;
				case '1' : // white
					layerDecoded[j] = '1';
					break;
			}
		}
		if (zeroCount < lastCount) {
			lastCount = zeroCount;
			strncpy(fewestZeroes, arrPtr+cnt, 150);
		}
	}

	int oneCount = 0, twoCount = 0;
	for(int i = 0; i < 150; i++) {
		if (fewestZeroes[i] == '1') oneCount++;
		else if (fewestZeroes[i] == '2') twoCount++;
	}
	printf("Part 1 --- Count is: %d.\n", oneCount * twoCount);

	char *ptr = layerDecoded;
	for(int i = 0; i < 6; i++) {
		for (int j = 0; j < 25; j++) {
			if (ptr[(25*i)+j] == '0') printf(" ");
			else printf("%c", ptr[(25*i)+j]);
		}
		printf("\n");
	}
	return 0;
}
