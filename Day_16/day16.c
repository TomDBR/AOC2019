#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int between(int val, int min, int max) {
	return val <= max && val >= min;
}

int main() {
	FILE *f = fopen("./input.txt", "r");
	char c[2] = { ' ', '\0' };
	int *phase = NULL, *newPhase = NULL, phaseLen = 0;
	int basePattern[4] = { 0,1,0,-1 };

	for (int i = 0; i < 10000; i++, fseek(f, 0, SEEK_SET)) {
		while ((c[0] = fgetc(f)) != EOF) {
			if (!between((int) c[0], (int) '0', (int) '9')) continue;
			phase = realloc(phase, ++phaseLen * sizeof(int));
			phase[phaseLen-1] = atoi(c);
		}
	}
	newPhase = calloc(phaseLen, sizeof(int));
	char offset[8]; offset[7] = '\0';
	for (int i = 0; i < 7; i++) 
		offset[i] = fgetc(f);
	int offsetNumeric = atoi(offset);
	fclose(f);

	int part1Size = phaseLen / 10000;
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < part1Size; j++) {
			int sizePattern = (j+1)*4; 
			int pattern[sizePattern];
			for (int pt = 0; pt < sizePattern; pt++) 
				pattern[pt] = basePattern[pt / (sizePattern / 4)];
			int sum = 0;
			for (int idx = j, patternIdx = j + 1; idx < part1Size; idx++, patternIdx++) {
				if (patternIdx == sizePattern) patternIdx = 0;
				sum += (pattern[patternIdx] * phase[idx]);
			}
			newPhase[j] = abs(sum) % 10;
		}
		for (int idx = 0; idx < part1Size; idx++) {
			phase[idx] = newPhase[idx];
		}
	}
	printf("PART 1: after %d phases of FFT, first 8 digits:  ", 100);
	for (int idx = 0; idx < 8; idx++) printf("%d", phase[idx]);
	printf("\n");

	for (int i = 0; i < 100; i++) {
		int sum = 0;
		for (int j = phaseLen -1; j >= offsetNumeric; j--) {
			sum += phase[j];
			newPhase[j] = abs(sum) % 10;
		}
		for (int idx = offsetNumeric; idx < phaseLen; idx++) {
			phase[idx] = newPhase[idx];
		}
	}
	printf("PART 2: after %d phases of FFT, 8-digit message: ", 100);
	for (int idx = offsetNumeric; idx < offsetNumeric + 8; idx++) printf("%d", phase[idx]);
	printf("\n");

	free(phase); free(newPhase);
	return 0;
}
