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
int main() {
	FILE *f = fopen("./input.txt", "r");
	char c;
	int *phase = NULL, *newPhase = NULL, phaseLen = 0;
	int basePattern[4] = { 0,1,0,-1 };
	while ((c = fgetc(f)) != EOF) {
		phase = realloc(phase, ++phaseLen * sizeof(int));
		phase[phaseLen-1] = atoi(&c);
	}
	newPhase = malloc(phaseLen * sizeof(int));
	fclose(f);

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < phaseLen; j++) {
			int *pattern = getPattern(basePattern,j+1);
			int patternIdx = 1, sizePattern = (j+1) * 4;
			printf("sizeof pattern: %d\n", sizePattern);
			int sum = 0;
			for (int idx = 0; idx < phaseLen; idx++) {
				//printf("%d*%d + ", pattern[patternIdx], phase[idx]);
				sum += (pattern[patternIdx] * phase[idx]);
				patternIdx = patternIdx == sizePattern - 1 ? 0 : patternIdx+1;
			}
			//printf(" = sum is: %d => %d\n", sum, abs(sum) % 10);
			newPhase[j] = abs(sum) % 10;
			free(pattern);
		}
		for (int idx = 0; idx < phaseLen; idx++)
			phase[idx] = newPhase[idx];
	}
	printf("after %d phases: ", 100);
	for (int idx = 0; idx < 8; idx++) {
		printf("%d", phase[idx]);
	}
	printf("\n");
	free(phase); free(newPhase);
	return 0;
}
