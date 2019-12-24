#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define verbosity 0

static struct wire *wires[2];

struct wire {
	struct coord **coordinates;
	struct coord **intersections;
	int amtCoords;
	int amtIntersections;
};

struct coord {
	int X;
	int Y;
};

struct line {
	struct coord *p1;
	struct coord *p2;
};

void cleanup() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < wires[i]->amtCoords; j++) {
			free(wires[i]->coordinates[j]);
		}
		free(wires[i]->coordinates);
		for (int j = 0; j < wires[i]->amtIntersections; j++) {
			free(wires[i]->intersections[j]);
		}
		free(wires[i]->intersections);
		free(wires[i]);
	}
}
struct wire *createWire() 
{
	struct wire *wire = malloc(sizeof(struct wire));
	wire->coordinates = NULL;
	wire->intersections = NULL;
	wire->amtCoords = 0;
	wire->amtIntersections = 0;
	return wire;
}

struct line *createLine(struct coord *c1, struct coord *c2) {
	struct line *tmp = malloc(sizeof(struct line));
	tmp->p1 = c1;
	tmp->p2 = c2;
	return tmp;
}

struct coord *createCoord(int x, int y) 
{
	struct coord *tmp = calloc(1, sizeof(struct coord));
	tmp->X = x;
	tmp->Y = y;
	return tmp;
}

void addCoord(struct wire *wire, struct coord *coord) 
{
	wire->coordinates = (struct coord **) realloc(wire->coordinates, (wire->amtCoords+1) * sizeof(struct coord *));
	wire->coordinates[wire->amtCoords] = coord;
	wire->amtCoords += 1;
}

void addIntersection(struct wire *wire, struct coord *coord) 
{
	wire->intersections = (struct coord **) realloc(wire->intersections, (wire->amtIntersections+1) * sizeof(struct coord *));
	wire->intersections[wire->amtIntersections] = coord;
	wire->amtIntersections += 1;
}

int sameAxis(struct line *line, char axis) 
{
	switch (axis) {
		case 'x' :
			return line->p1->X == line->p2->X;
		case 'y' :
			return line->p1->Y == line->p2->Y;
	}
	return 0;
}

int parallel(struct line *line1, struct line *line2) {
	if((sameAxis(line1, 'x') && sameAxis(line2, 'x')) || (sameAxis(line1, 'y') && sameAxis(line2, 'y'))) {
		return 1;
	}
	else return 0;
}

void loadData() 
{
	char inputLine[1500], *inPtr = inputLine;
	memset(inputLine, '\0', 1500);
	FILE *ifp;
	int lineCount = 0;

	if ((ifp = fopen("./03_input.txt", "r")) == NULL) exit(1);
	while(fscanf(ifp, "%s\n,", inputLine)) {
		struct wire *wire = createWire();
		int increment = 0;
		char direction, *inPtr = inputLine;
		addCoord(wire, createCoord(0,0));
		while(sscanf(inPtr, "%c%d,", &direction, &increment) == 2) {
			int XPosition = (wire->amtCoords) ? wire->coordinates[wire->amtCoords-1]->X : 0;
			int YPosition = (wire->amtCoords) ? wire->coordinates[wire->amtCoords-1]->Y : 0;
			switch(direction) {
				case 'U' :
					addCoord(wire, createCoord(XPosition, YPosition + increment));
					break;
				case 'D' :
					addCoord(wire, createCoord(XPosition, YPosition - increment));
					break;
				case 'R' :
					addCoord(wire, createCoord(XPosition + increment, YPosition));
					break;
				case 'L' :
					addCoord(wire, createCoord(XPosition - increment, YPosition));
					break;
				default :
					fprintf(stderr, "invalid direction %c!\n", direction);

			}
			inPtr += getAmountOfChars(increment) + 2;
		}
		wires[lineCount] = wire;
		lineCount++;
		if (feof(ifp)) break;
	}
	fclose(ifp);

}

void findIntersections() 
{
	for (int i = 0; i < wires[0]->amtCoords-1; i++) { // w1c1 -> wire1 coord 1
		struct coord *w1c1 = wires[0]->coordinates[i], *w1c2 = wires[0]->coordinates[i+1];
		struct line *line1 = createLine(w1c1, w1c2);
		for (int j = 0; j < wires[1]->amtCoords-1; j++) { 
			struct coord *w2c1 = wires[1]->coordinates[j], *w2c2 = wires[1]->coordinates[j+1];
			struct line *line2 = createLine(w2c1, w2c2);
			if (!parallel(line1, line2)) {
				if (sameAxis(line1, 'x') && sameAxis(line2, 'y')) {
					if (between(line1->p1->X, line2->p1->X, line2->p2->X) && between(line2->p1->Y, line1->p1->Y, line1->p2->Y)) {
						addIntersection(wires[0], createCoord(line1->p1->X, line2->p1->Y));
						addIntersection(wires[1], createCoord(line1->p1->X, line2->p1->Y));
					}
				} else if (sameAxis(line1, 'y') && sameAxis(line2, 'x')) {
					if (between(line2->p1->X, line1->p1->X, line1->p2->X) && between(line1->p1->Y, line2->p1->Y, line2->p2->Y)) {
						addIntersection(wires[0], createCoord(line2->p1->X, line1->p1->Y));
						addIntersection(wires[1], createCoord(line2->p1->X, line1->p1->Y));
					}
				}
			}
			free(line2);
		}
		free(line1);
	}
}

int findFewestSteps() {
	int fewestSteps = 1000000;
	for (int i = 0; i < wires[0]->amtIntersections; i++) {
		struct coord *intersection = wires[0]->intersections[i];
		if (verbosity) printf("checking intersection: (%d,%d)..\n", intersection->X, intersection->Y);
		int steps = 0;
		for (int x = 0; x < 2; x++) {
			struct coord *start = wires[x]->coordinates[0];
			for (int y = 1; y < wires[x]->amtCoords; y++) {
				struct coord *cur = wires[x]->coordinates[y];
				struct line *tmp = createLine(start, cur);
				if (sameAxis(tmp, 'x')) {
					if (between(intersection->Y, start->Y, cur->Y) && intersection->X == tmp->p1->X) { // intersection reached
						steps += abs(intersection->Y - start->Y); 
						free(tmp); break;
					}
					steps += abs((cur->Y - (start->Y))); 
				} else if (sameAxis(tmp, 'y')) {
					if (between(intersection->X, start->X, cur->X) && intersection->Y == tmp->p1->Y) { // intersection reached
						steps += abs((intersection->X - (start->X))); 
						free(tmp); break;
					}
					steps += abs((cur->X - (start->X))); 
				}
				start = cur;
				free(tmp);
			}
		}
		if ((steps) < fewestSteps) fewestSteps = steps;
	}
	return fewestSteps;
}

int main(int argc, char* argv[]) {
	loadData();
	findIntersections();

	int shortestDistance = abs(wires[0]->intersections[0]->X) + abs(wires[0]->intersections[0]->Y);
	for (int i = 0; i < wires[0]->amtIntersections; i++) {
		int distance = abs(wires[0]->intersections[i]->X) + abs(wires[0]->intersections[i]->Y);
		if (distance < shortestDistance && distance != 0) shortestDistance = distance;
	}
	printf("shortest distance: %d\n", shortestDistance);
	printf("fewest steps: %d\n", findFewestSteps());
	cleanup();
	return 0;
}
