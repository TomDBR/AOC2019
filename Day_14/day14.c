#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char 	*fileName = "./14_input.txt";
static struct 	equation **eq_system = NULL;
static int 	eq_systemSize = 0;

struct chemical {
	unsigned long long amount;
	char *name;
	int leftovers;
};

struct equation { // 2xTYPE = 7xTYPE + 4xTYPE
	struct chemical *leftTerm;
	struct chemical **rightTerms;
	int rightArgCnt;
};

void printOre(struct chemical *chemical, int withNewline) 
{
	withNewline ? printf("%lli%s\n", chemical->amount, chemical->name) : printf("%llix%s", chemical->amount, chemical->name); 
}

void printVgl(struct equation *eq) 
{
	printOre(eq->leftTerm, 0);
	printf(" = ");
	for (int j = 0; j < eq->rightArgCnt; j++) {
		printOre(eq->rightTerms[j], 0);
		j == eq->rightArgCnt -1 ? printf("\n") : printf(" + ");
	}
}

void eqMultiply(struct equation *eq, int num) 
{
	eq->leftTerm->amount *= num;
	for (int i = 0; i < eq->rightArgCnt; i++) {
		eq->rightTerms[i]->amount *= num;
	}
}

void eqDivide(struct equation *eq, int num) 
{
	eq->leftTerm->amount /= num;
	for (int i = 0; i < eq->rightArgCnt; i++) {
		eq->rightTerms[i]->amount = ceil((double) eq->rightTerms[i]->amount / (double) num);
	}
}

void deleteOrefromVgl(struct equation *eq, char *delet) 
{
	for (int i = 0; i < eq->rightArgCnt; i++) {
		if (strcmp(eq->rightTerms[i]->name, delet) == 0) {
			eq->rightArgCnt -= 1;
			eq->rightTerms[i] = eq->rightTerms[eq->rightArgCnt];
			eq->rightTerms[eq->rightArgCnt] = NULL;
		}
	}
}

struct chemical *createOre(int amount, char *name)
{
	struct chemical *newOre = calloc(1, sizeof(struct chemical));
	int len = strlen(name);
	if (name[len-1] == ',') name[len-1] = '\0';
	newOre->amount = amount;
	newOre->leftovers = 0;
	newOre->name = strdup(name);
	return newOre;
}

void concatOres(struct equation *eq, struct equation *toAdd) 
{
	for (int i = 0; i < toAdd->rightArgCnt; i++) {
		int alreadyPresent = 0;
		for (int x = 0; x < eq->rightArgCnt; x++) {
			if (strcmp(eq->rightTerms[x]->name, toAdd->rightTerms[i]->name) == 0) {
				alreadyPresent = 1;
				eq->rightTerms[x]->amount += toAdd->rightTerms[i]->amount;
				break;
			}
		}
		if (!alreadyPresent) {
			eq->rightArgCnt = eq->rightArgCnt +1;
			eq->rightTerms = (struct chemical **) realloc(eq->rightTerms, eq->rightArgCnt * sizeof(struct chemical *));
			eq->rightTerms[eq->rightArgCnt-1] = createOre(toAdd->rightTerms[i]->amount, toAdd->rightTerms[i]->name);
		}
	}
}

void addToVgl(struct equation *eq, struct equation *eqToAdd, int i)
{
	struct chemical *chemical_in_eq = eq->rightTerms[i];
	int amt_in_eq = chemical_in_eq->amount - eqToAdd->leftTerm->leftovers;

	if (amt_in_eq < 0) {
		deleteOrefromVgl(eq, chemical_in_eq->name);
		eqToAdd->leftTerm->leftovers -= chemical_in_eq->amount;
		return;
	} else {
		eqToAdd->leftTerm->leftovers = 0;
	}
	int name_amt = eqToAdd->leftTerm->amount;
	int mod = amt_in_eq % name_amt;
	int amount = mod == 0 ? amt_in_eq / name_amt : ( amt_in_eq / name_amt ) + 1;

	if (mod) eqToAdd->leftTerm->leftovers = name_amt - mod;
	
	eqMultiply(eqToAdd, amount);
	printf("\t\t"); printVgl(eq); printf("\t+ \t"); printVgl(eqToAdd); printf("\t\tleft: %d\n", eqToAdd->leftTerm->leftovers);
	deleteOrefromVgl(eq, chemical_in_eq->name);
	concatOres(eq, eqToAdd);
	eqDivide(eqToAdd, amount);
}

struct equation *findVgl(char *name)
{
	for (int i = 0; i < eq_systemSize; i++) {
		if (strcmp(eq_system[i]->leftTerm->name, name) == 0) return eq_system[i];
	}
	return NULL;
}

void addVgl(struct chemical *main, struct chemical **list, int listSize) 
{
	eq_systemSize++;
	if (eq_system == NULL) eq_system = calloc(1, sizeof(struct equation *));
	else eq_system = (struct equation **) realloc(eq_system, eq_systemSize * sizeof(struct equation *));
	eq_system[eq_systemSize-1] = malloc(sizeof(struct equation));
	eq_system[eq_systemSize-1]->leftTerm = main;
	eq_system[eq_systemSize-1]->rightTerms = list;
	eq_system[eq_systemSize-1]->rightArgCnt = listSize;
}

void loadData() 
{
	FILE *file = fopen(fileName, "r");
	char chemicalAmount[10], chemicalName[10];
	struct chemical **list = NULL; int rightArgCnt = 0;
	while (fscanf(file, "%s", chemicalAmount)) {
		if(feof(file)) break;
		if(strcmp(chemicalAmount, "=>") == 0) {
			fscanf(file, "%s%s", chemicalAmount, chemicalName);
			struct chemical *main = createOre(atoi(chemicalAmount), chemicalName);
			addVgl(main, list, rightArgCnt);
			list = NULL; rightArgCnt = 0;
		} else {
			fscanf(file, "%s", chemicalName);
			struct chemical *newOre = createOre(atoi(chemicalAmount), chemicalName);
			rightArgCnt++;
			list = (struct chemical **) realloc(list, rightArgCnt * sizeof(struct chemical *));
			list[rightArgCnt-1] = newOre;
		}
	}
	fclose(file);
}

int main()
{
	loadData();
	struct equation *masterVgl = findVgl("FUEL");
	while (1) {
		int somethingHappened = 0;
		for (int i = 0; i < masterVgl->rightArgCnt; i++) {
			struct equation *tmp = findVgl(masterVgl->rightTerms[i]->name);
			if (findVgl(tmp->rightTerms[0]->name) != NULL) { // chemical
				somethingHappened = 1;
				printf("next addition ==============================\n");
				addToVgl(masterVgl, tmp, i);
				printf("\t------------------------------------\n");
				printf("\t=\t");
				printVgl(masterVgl);
			}
		}
		if (!somethingHappened) break;
	}

	printVgl(masterVgl);
	eqDivide(masterVgl, masterVgl->leftTerm->amount);
	printVgl(masterVgl);
	int total = 0;
	for (int i = 0; i < masterVgl->rightArgCnt; i++) {
		struct equation *base_eq = findVgl(masterVgl->rightTerms[i]->name);
		int base_amount = base_eq->leftTerm->amount;
		int chemical_amount = base_eq->rightTerms[0]->amount;
		int master_amount = masterVgl->rightTerms[i]->amount;
		int rounding_addition = master_amount % base_amount ? (base_amount - (master_amount % base_amount)) : 0;
		int rounded_amount = master_amount + rounding_addition;
		rounded_amount /= base_eq->leftTerm->amount;
		int amountOfOre = rounded_amount * chemical_amount;
		printf("rounded amount: %d\tamount of chemical: %d\t", rounded_amount, amountOfOre);
		printVgl(base_eq);
		total += amountOfOre;

	}
	printf("total is: %d\n", total);
	return 0;
}
