#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char 	*fileName = "./14_input.txt";
static struct 	equation **stelsel = NULL;
static int 	stelselSize = 0;

struct ore 
{
	unsigned long long amount;
	char *type;
};

struct equation 
{ // 2xTYPE = 7xTYPE + 4xTYPE
	struct ore *leftTerm;
	struct ore **rightTerms;
	int rightArgCnt;
};

void printOre(struct ore *ore, int withNewline) 
{
	withNewline ? printf("%lli%s\n", ore->amount, ore->type) : printf("%llix%s", ore->amount, ore->type); 
}

void printVgl(struct equation *vgl) 
{
	printOre(vgl->leftTerm, 0);
	printf(" = ");
	for (int j = 0; j < vgl->rightArgCnt; j++) {
		printOre(vgl->rightTerms[j], 0);
		j == vgl->rightArgCnt -1 ? printf("\n") : printf(" + ");
	}
}

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

int greatestCommonDivisor(int x, int y) 
{
	// Euclidean algorithm!
	x = abs(x);
	y = abs(y);
	if (x == 0 || y == 0) return 1;
	int biggestValue = x >= y ? x : y;
	int smallestValue = x < y ? x : y;
	while(1) {
		int count = 0;
		while (biggestValue >= smallestValue) {
			biggestValue -= smallestValue;
			count++;
		}
		if (biggestValue == 0) return smallestValue;
		else if (biggestValue < 1) return 0;
		swap(&biggestValue, &smallestValue);
	}
	return 1;
}

void vglMultiply(struct equation *vgl, int num) 
{
	vgl->leftTerm->amount *= num;
	for (int i = 0; i < vgl->rightArgCnt; i++) {
		vgl->rightTerms[i]->amount *= num;
	}
}

void vglDivide(struct equation *vgl, int num) 
{
	vgl->leftTerm->amount /= num;
	for (int i = 0; i < vgl->rightArgCnt; i++) {
		vgl->rightTerms[i]->amount = ceil((double) vgl->rightTerms[i]->amount / (double) num);
	}
}

void deleteOrefromVgl(struct equation *vgl, char *delet) 
{
	for (int i = 0; i < vgl->rightArgCnt; i++) {
		if (strcmp(vgl->rightTerms[i]->type, delet) == 0) {
			vgl->rightArgCnt -= 1;
			vgl->rightTerms[i] = vgl->rightTerms[vgl->rightArgCnt];
			vgl->rightTerms[vgl->rightArgCnt] = NULL;
		}
	}
}

struct ore *createOre(int amount, char *name)
{
	struct ore *newOre = calloc(1, sizeof(struct ore));
	int len = strlen(name);
	if (name[len-1] == ',') name[len-1] = '\0';
	newOre->amount = amount;
	newOre->type = strdup(name);
	return newOre;
}

void concatOres(struct equation *vgl, struct equation *toAdd) 
{
	for (int i = 0; i < toAdd->rightArgCnt; i++) {
		int alreadyPresent = 0;
		for (int x = 0; x < vgl->rightArgCnt; x++) {
			if (strcmp(vgl->rightTerms[x]->type, toAdd->rightTerms[i]->type) == 0) {
				alreadyPresent = 1;
				vgl->rightTerms[x]->amount += toAdd->rightTerms[i]->amount;
				break;
			}
		}
		if (!alreadyPresent) {
			vgl->rightArgCnt = vgl->rightArgCnt +1;
			vgl->rightTerms = (struct ore **) realloc(vgl->rightTerms, vgl->rightArgCnt * sizeof(struct ore *));
			vgl->rightTerms[vgl->rightArgCnt-1] = createOre(toAdd->rightTerms[i]->amount, toAdd->rightTerms[i]->type);
		}
	}
}

void addToVgl(struct equation *vgl, struct equation *vglToAdd)
{
	for (int i = 0; i < vgl->rightArgCnt; i++) {
		struct ore *tmp = vgl->rightTerms[i];
		if (strcmp(tmp->type, vglToAdd->leftTerm->type) == 0) {
			int gcd 	= greatestCommonDivisor(tmp->amount, vglToAdd->leftTerm->amount);
			int vglAmt 	= vglToAdd->leftTerm->amount / gcd;
			int toAddAmt 	= tmp->amount / gcd;
			vglMultiply(vglToAdd, toAddAmt);
			vglMultiply(vgl, vglAmt);
			printf("\t\t");
			printVgl(vgl); 
			printf("\t+ \t");
			printVgl(vglToAdd);
			deleteOrefromVgl(vgl, tmp->type);
			concatOres(vgl, vglToAdd);
			vglDivide(vglToAdd, toAddAmt);
			break;
		}
	}
}

struct equation *findVgl(char *type)
{
	for (int i = 0; i < stelselSize; i++) {
		if (strcmp(stelsel[i]->leftTerm->type, type) == 0) return stelsel[i];
	}
	return NULL;
}

void addVgl(struct ore *main, struct ore **list, int listSize) 
{
	stelselSize++;
	if (stelsel == NULL) stelsel = calloc(1, sizeof(struct equation *));
	else stelsel = (struct equation **) realloc(stelsel, stelselSize * sizeof(struct equation *));
	stelsel[stelselSize-1] = malloc(sizeof(struct equation));
	stelsel[stelselSize-1]->leftTerm = main;
	stelsel[stelselSize-1]->rightTerms = list;
	stelsel[stelselSize-1]->rightArgCnt = listSize;
}

void loadData() 
{
	FILE *file = fopen(fileName, "r");
	char oreAmount[10], oreName[10];
	struct ore **list = NULL; int rightArgCnt = 0;
	while (fscanf(file, "%s", oreAmount)) {
		if(feof(file)) break;
		if(strcmp(oreAmount, "=>") == 0) {
			fscanf(file, "%s%s", oreAmount, oreName);
			struct ore *main = createOre(atoi(oreAmount), oreName);
			addVgl(main, list, rightArgCnt);
			list = NULL; rightArgCnt = 0;
		} else {
			fscanf(file, "%s", oreName);
			struct ore *newOre = createOre(atoi(oreAmount), oreName);
			rightArgCnt++;
			list = (struct ore **) realloc(list, rightArgCnt * sizeof(struct ore *));
			list[rightArgCnt-1] = newOre;
		}
	}
	fclose(file);
}

int main(int argc, char *argv[]) 
{
	loadData();
	struct equation *masterVgl = findVgl("FUEL");
	while (1) {
		int somethingHappened = 0;
		for (int i = 0; i < masterVgl->rightArgCnt; i++) {
			struct equation *tmp = findVgl(masterVgl->rightTerms[i]->type);
			if (findVgl(tmp->rightTerms[0]->type) != NULL) { // ore
				somethingHappened = 1;
				printf("next addition ==============================\n");
				addToVgl(masterVgl, tmp);
				printf("\t------------------------------------\n");
				printf("\t=\t");
				printVgl(masterVgl);
			}
		}
		if (!somethingHappened) break;
	}

	printVgl(masterVgl);
	vglDivide(masterVgl, masterVgl->leftTerm->amount);
	printVgl(masterVgl);
	int total = 0;
	for (int i = 0; i < masterVgl->rightArgCnt; i++) {
		struct equation *tmp = findVgl(masterVgl->rightTerms[i]->type);
		int roundedAmt = ceil((double) masterVgl->rightTerms[i]->amount / (double) tmp->leftTerm->amount);
		printf("rounded amount: %d\t", roundedAmt);
		int amountOfOre = roundedAmt * tmp->rightTerms[0]->amount;
		printf("amount of ore: %d\t", amountOfOre);
		printVgl(tmp);
		total += amountOfOre;

	}
	printf("total is: %d\n", total);
	return 0;
}
