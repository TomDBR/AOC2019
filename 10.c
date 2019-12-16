#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>

#define verbosity 0
#define W 840
#define H 480


#define abs(a)		(a < 0 ? -a : a)

static struct map {
	int xSize;
	int ySize;
	struct coord {
		int x;
		int y;
		struct asteroid *asteroid;
	} **coordinates;
	int AmtCoords;
} *map = NULL;

struct asteroid {
	struct asteroid **visibleAsteroids;
	struct coord *coordinate;
	int amount;
};

struct slope {
	int x;
	int y;
};

static struct slopeArray {
	struct slope **Q1Slopes;
	struct slope **Q2Slopes;
	struct slope **Q3Slopes;
	struct slope **Q4Slopes;
	struct slope **totalArray;
	int amountQ1;
	int amountQ2;
	int amountQ3;
	int amountQ4;
	int totalAmount;
} *slopeArray = NULL;

void die(char *msg)
{
	printf("%s\n", msg);
	exit(1);
}

void swap(int *one, int *two) 
{
	int tmp = *one;
	*one = *two;
	*two = tmp;
}

void slopeSwap(struct slope *one, struct slope *two) 
{
	int tmpX = one->x;
	int tmpY = one->y;
	one->x = two->x;
	one->y = two->y;
	two->x = tmpX;
	two->y = tmpY;
}

double calculateSlope(struct slope *slope) 
{
	return (double) slope->y / (double) slope->x;
}

void createMap()
{
	map = malloc(sizeof(struct map));
	map->coordinates = malloc(sizeof(struct coord *));
	map->xSize = 0;
	map->ySize = 0;
	map->AmtCoords = 0;
}

struct coord *addToMap(int x, int y) 
{
	int newSize = map->AmtCoords + 1;
	map->coordinates = (struct coord **) realloc(map->coordinates, newSize * sizeof(struct coord *));
	(map->coordinates)[newSize-1] = malloc(sizeof(struct coord));
	memset((map->coordinates)[newSize-1], 0, sizeof(struct coord));
	struct coord **coords = map->coordinates;
	(coords[newSize-1])->x = x;
	(coords[newSize-1])->y = y;
	map->AmtCoords = newSize;
	return map->coordinates[newSize-1];
}

int withinBoundaries(int x, int y) 
{
	return x < map->xSize && y < map->ySize && y >= 0 && x >= 0 ? 1 : 0;
}

struct asteroid *createAsteroid(struct asteroid **array, int amount, struct coord *coordinate)
{
	struct asteroid *new = malloc(sizeof(struct asteroid));
	memset(new, 0, sizeof(struct asteroid));
	if (new == NULL) return NULL;
	new->visibleAsteroids = array;
	new->coordinate = coordinate;
	new->amount = amount;
	return new;
}

int addToArray(struct asteroid *main, struct asteroid *objToAdd)
{
	int newSize = main->amount + 1;
	if (main->visibleAsteroids == NULL) {
		if ((main->visibleAsteroids = malloc(sizeof(struct asteroid *))) == NULL) return 0;
	} else {
		if ((main->visibleAsteroids = (struct asteroid **) realloc(main->visibleAsteroids, newSize * sizeof(struct asteroid *))) == NULL) return 0;
	}
	main->visibleAsteroids[newSize-1] = objToAdd;
	main->amount = newSize;
	return 1;
}

struct asteroid *getAsteroid(int x, int y) 
{
	return withinBoundaries(x,y) ? map->coordinates[(map->xSize)*y+x]->asteroid : NULL;
}

void toString(struct coord *coord)
{
	printf("%p: (%02d:%02d)\t| ", coord, coord->x, coord->y);
	coord->asteroid != NULL ? printf("%d visible Asteroids\n", coord->asteroid->amount) : printf("\n");
}

static void loadData() 
{
	FILE *file = fopen("./10_input.txt", "r");
	if (!file) die("failed to open file!");
	createMap();
	int x = 0, y = 0;
	while(1) {
		char cur = fgetc(file);
		if(feof(file)) break;
		switch(cur) {
			case '.' : 
				   addToMap(x, y);
				   x++;
				   break;
			case '#' : ;
				   struct coord *tmp = addToMap(x, y);
				   (map->coordinates)[map->AmtCoords-1]->asteroid = createAsteroid(NULL, 0, tmp);
				   x++;
				   break;
			case '\n' :
				   x=0;
				   y++;
		}
	}
	// TODO if x != y this calculation won't work, i need to check which value is smallest!
	map->xSize = map->AmtCoords / y;
	map->ySize = y;
	fclose(file);
}

void cleanUp() 
{
	for (int i = 0; i < map->AmtCoords; i++) {
		if (map->coordinates[i]->asteroid != NULL) {
			free(map->coordinates[i]->asteroid->visibleAsteroids);
			free(map->coordinates[i]->asteroid);
		}
		free(map->coordinates[i]);
	}
	free(map->coordinates);
	free(map);
	for (int i = 0; i < slopeArray->totalAmount; i++) {
		free(slopeArray->totalArray[i]);
	}
	free(slopeArray->totalArray);
	free(slopeArray->Q1Slopes);
	free(slopeArray->Q2Slopes);
	free(slopeArray->Q3Slopes);
	free(slopeArray->Q4Slopes);
	free(slopeArray);
}

int partition (struct slope **arr, int low, int high, int ascending)
{
    double pivot = calculateSlope(arr[high]);
    int i = low - 1;
    for (int j = low; j <= high- 1; j++) {
        if ((!ascending && calculateSlope(arr[j]) < pivot) || (ascending && calculateSlope(arr[j]) > pivot)) {
            i++;
            slopeSwap(arr[i], arr[j]);
        }
    }
    slopeSwap(arr[i + 1], arr[high]);
    return i + 1;
}

// ascending = 0 -> sort in descending order
void quickSort(struct slope **arr, int low, int high, int ascending)
{
    if (low < high) {
        int pi = partition(arr, low, high, ascending);
        quickSort(arr, low, pi - 1, ascending);
        quickSort(arr, pi + 1, high, ascending);
    }
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

int uniqueSlope(struct slope **array, int *size, int y, int x) 
{
	for (int i = 0; i < *size; i++) {
		if (array[i]->x == x && array[i]->y == y) return 0;
	}
	return 1;
}

int addSlope(struct slope ***array, int *size, int y, int x)
{
	// if one of the numbers is zero, taking gcd won't work, so we divide by itself
	if (x == 0 && y == 0) die("This should never happen!");
	else if (x == 0) { 
		if (y < 0) y /= -y;
		else y /= y;
	} else if (y == 0) {
		if (x < 0) x /= -x;
		else x /= x;
	} else {
		int gcd = greatestCommonDivisor(abs(x), abs(y));
		x = x / gcd;
		y = y / gcd;
	}
	if (!uniqueSlope(*array, size, y, x)) return 0;
	int newSize = *size + 1;
	if (*array == NULL) {
		if ((*array = malloc(sizeof(struct slope *))) == NULL) return 0;
	} else {
		if ((*array = (struct slope **) realloc(*array, newSize * sizeof(struct slope *))) == NULL) return 0;
	}
	struct slope *tmp = malloc(sizeof(struct slope));
	tmp->x = x;
	tmp->y = y;
	(*array)[newSize-1] = tmp;
	*size = newSize;
	return 1;
}

void getSlopesForQuadrant(struct slope ***slopes, struct coord *coord, int *cnt, int y2, int x2)
{
	int xDiff = 0, yDiff = 0, changeX = 0, changeY = 0, xUpper = 0, yUpper = 0;
	xDiff = x2 - coord->x; // 2
	yDiff = y2 - coord->y; // -2
	changeX = xDiff >= 0 ? 1 : -1;
	changeY = yDiff >= 0 ? 1 : -1;
	xUpper = changeX * xDiff;
	yUpper = changeY * yDiff;
	if (verbosity) 
		printf("xDiff = %d. yDiff = %d, changeX: %d, changeY: %d, Xupper: %d, yUpper: %d\n", xDiff, yDiff, changeX, changeY, xUpper, yUpper);

	int i1 = 1, j1 = yUpper, i2 = 1, j2 = yUpper;
	if (yDiff != 0) addSlope(slopes, cnt, yDiff, 0);
	for(int i1 = 1; i1 <= xUpper; i1++) {
		for(int j1 = yUpper; j1 >= 1; j1--) {
			addSlope(slopes, cnt, j1*changeY, i1*changeX);
		}
	}
}

struct coord *getCoord(int x, int y) 
{
	return withinBoundaries(x,y) ? map->coordinates[(map->xSize)*y+x] : NULL;
}

void populateSlopeArray() 
{ 
	struct coord *coord = NULL;
	int minY = 0, maxY = map->xSize-1, minX = 0, maxX = map->ySize-1;
	int Q1Size = 0, Q2Size = 0, Q3Size = 0, Q4Size = 0;

	slopeArray = malloc(sizeof(struct slopeArray));
	slopeArray->amountQ1 = 0;
	slopeArray->amountQ2 = 0;
	slopeArray->amountQ3 = 0;
	slopeArray->amountQ4 = 0;
	slopeArray->Q1Slopes = malloc(sizeof(struct slope *));
	slopeArray->Q2Slopes = malloc(sizeof(struct slope *));
	slopeArray->Q3Slopes = malloc(sizeof(struct slope *));
	slopeArray->Q4Slopes = malloc(sizeof(struct slope *));

	// slope formula: s = Δy / Δx
	// Δy = y2 - y1
	// Δx = x2 - x1
	// x1, y1 given by coord, x2, y2 specified, either minX, minY, maxX, maxY
	
	// Q1
	coord = getCoord(minX, maxY);
	getSlopesForQuadrant(&slopeArray->Q1Slopes, coord, &slopeArray->amountQ1, minY, maxX);
	quickSort(slopeArray->Q1Slopes, 0, slopeArray->amountQ1-1, 0);
	Q1Size = slopeArray->amountQ1;
	// Q2
	coord = getCoord(minX, minY);
	getSlopesForQuadrant(&slopeArray->Q2Slopes, coord, &slopeArray->amountQ2, maxY, maxX);
	quickSort(slopeArray->Q2Slopes, 0, slopeArray->amountQ2-1, 1);
	Q2Size = Q1Size + slopeArray->amountQ2;
	// Q3
	coord = getCoord(maxX, minY);
	getSlopesForQuadrant(&slopeArray->Q3Slopes, coord, &slopeArray->amountQ3, maxY, minX);
	quickSort(slopeArray->Q3Slopes, 0, slopeArray->amountQ3-1, 1);
	Q3Size = Q2Size + slopeArray->amountQ3;
	// Q4
	coord = getCoord(maxX, maxY);
	getSlopesForQuadrant(&slopeArray->Q4Slopes, coord, &slopeArray->amountQ4, minY, minX);
	quickSort(slopeArray->Q4Slopes, 0, slopeArray->amountQ4-1, 0);
	Q4Size = Q3Size + slopeArray->amountQ4;

	slopeArray->totalAmount = Q4Size;
	slopeArray->totalArray = malloc(Q4Size * sizeof(struct slope *));

	for (int i = 0; i < Q1Size; i++) 			slopeArray->totalArray[i] = slopeArray->Q1Slopes[i];
	for (int i = 0, j = Q1Size; j < Q2Size; i++, j++) 	slopeArray->totalArray[j] = slopeArray->Q2Slopes[i];
	for (int i = 0, j = Q2Size; j < Q3Size; i++, j++) 	slopeArray->totalArray[j] = slopeArray->Q3Slopes[i];
	for (int i = 0, j = Q3Size; j < Q4Size; i++, j++) 	slopeArray->totalArray[j] = slopeArray->Q4Slopes[i];
}


void populateAsteroidMatches() 
{
	for (int i = 0; i < map->AmtCoords; i++) {
		struct coord *iCoord = map->coordinates[i];
		struct asteroid *iAsteroid = iCoord->asteroid;
		if (iAsteroid != NULL) {  
			if (verbosity) printf("X coord = %d, Y coord = %d.\n", iCoord->x, iCoord->y);
			for (int j = 0; j < slopeArray->totalAmount; j++) {
				int slopeX = slopeArray->totalArray[j]->x, slopeY = slopeArray->totalArray[j]->y; // size of the steps
				int initialX = iCoord->x; int initialY = iCoord->y; // starting position
				int x = initialX + slopeX; int y = initialY + slopeY; // iterated value
				while (withinBoundaries(x,y)) {
					if (verbosity) printf("X: %d->%d\t Y: %d->%d\n", initialX, x, initialY, y);
					struct asteroid *possibleHit = getAsteroid(x, y);
					if (possibleHit != NULL) {
						addToArray(iAsteroid, possibleHit);
						break;
					}
					x+=slopeX;
					y+=slopeY;
				}
			}
		}
	}
}

int main(int argc, char *argv[]) {
	loadData();
	populateSlopeArray();
	populateAsteroidMatches();

	struct coord *mostMatches = NULL;
  	for (int i = 0; i < map->AmtCoords; i++) {
		struct coord *currCoord = map->coordinates[i];
		if (currCoord->asteroid != NULL) {
			struct asteroid *asteroid = currCoord->asteroid;
			if (mostMatches == NULL) mostMatches = asteroid->coordinate;
			if (verbosity) toString(currCoord);
			if (asteroid->amount > mostMatches->asteroid->amount)
				mostMatches = currCoord;

		}
	}
	printf("RESULT =>\t");
	toString(mostMatches);
	printf("\n");

	printf("no\t| Coordinate\t\t\t| Amount of matches\n");
	printf("=====================================================================================\n");
	struct asteroid *tmp = mostMatches->asteroid;
	for (int i = 0; i < tmp->amount; i++) {
		printf("%d\t| ", i+1);
		toString(tmp->visibleAsteroids[i]->coordinate);
	}

	cleanUp();
	return 0;
}
