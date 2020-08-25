#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int *getPattern(int *base, int mul)
{
	int *pattern = NULL, size = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < mul; j++) {
			pattern = realloc(pattern, ++size * sizeof(int));
			pattern[size-1] = base[i];
		}
	}
	return pattern;
}
int between(int val, int min, int max) {
	return val <= max && val >= min;
}

int main() {
	FILE *f = fopen("./input.txt", "r");
	char c[2] = { ' ', '\0' };
	int *phase = NULL, *newPhase = NULL, phaseLen = 0;
	int basePattern[4] = { 0,1,0,-1 };
	//for (int i = 0; i < 10000; i++) {
		while ((c[0] = fgetc(f)) != EOF) {
			if (!between((int) c[0], (int) '0', (int) '9')) continue;
			phase = realloc(phase, ++phaseLen * sizeof(int));
			phase[phaseLen-1] = atoi(c);
		}
		fseek(f, 0, SEEK_SET);
	//}
	newPhase = calloc(phaseLen, sizeof(int));
	char offset[8]; offset[7] = '\0';
	for (int i = 0; i < 7; i++) offset[i] = fgetc(f);
	printf("offset is: %s\n", offset);
	fclose(f);


	for (int i = 0; i < 100; i++) {
		printf("PHASE %d\n", i+1);
		for (int j = 0; j < phaseLen; j++) {
			int *pattern = getPattern(basePattern,j+1);
			int patternIdx = 1, sizePattern = (j+1) * 4;
			int sum = 0;
			for (int idx = 0; idx < phaseLen; idx++) {
				sum += (pattern[patternIdx] * phase[idx]);
				patternIdx = patternIdx == sizePattern - 1 ? 0 : patternIdx+1;
			}
			newPhase[j] = abs(sum) % 10;
			free(pattern);
		}
		for (int idx = 0; idx < phaseLen; idx++)
			phase[idx] = newPhase[idx];
	}
	printf("after %d phases: ", 100);
	for (int idx = 0; idx < 8; idx++) printf("%d", phase[idx]);
	printf("\n");

	free(phase); free(newPhase);
	return 0;
}
