#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getAmountOfChars(int input) {
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

int abs(int arg) {
	return arg <= 0 ? -arg : arg;
}

int between(int arg, int outer1, int outer2) {
	if ((outer1 < arg && arg < outer2) || (outer2 < arg && arg < outer1)) 
		return 1;
	else 
		return 0;
}

int **findIntersections(int **carrier[2]) {
	int **intersections = malloc(2 * sizeof(int*));
	intersections[0] = malloc(100 * sizeof(int));
	memset(intersections[0], 0, 100 * sizeof(int));
	intersections[1] = malloc(100 * sizeof(int));
	memset(intersections[1], 0, 100 * sizeof(int));
	int iter = 0;

	int **line1 = carrier[0];
	int **line2 = carrier[1];
	for(int i = 0; i <= 299; i++) {
		int xi1 = line1[0][i];
		int yi1 = line1[1][i];
		int xi2 = line1[0][i+1];
		int yi2 = line1[1][i+1];
		//printf("i %d:\t%d.%d\t -> \t%d.%d\n", i, xi1, yi1, xi2, yi2);
		for(int j = 0; j <= 299; j++) {
			int xj1 = line2[0][j];
			int yj1 = line2[1][j];
			int xj2 = line2[0][j+1];
			int yj2 = line2[1][j+1];
			//printf("j %d:\t%d.%d\t -> \t%d.%d\n", j, xj1, yj1, xj2, yj2);

			int match1 = 0;
			int match2 = 0;
			if((xi1 == xi2 && xj1 == xj2) || (yi1 == yi2 && yj1 == yj2)) {
				// do nothing cuz perpendiclar??
			} else {
				if (xi1 == xi2) {
					match1 = xi1;
					match2 = yj1;
					if (between(match1, xj1, xj2) && between(match2, yi1, yi2)) {
						printf("match! (%d,%d) (%d,%d) -> (%d,%d)\t (%d,%d) -> (%d,%d)\n", match1, match2, xi1, yi1, xi2, yi2, xj1, yj1, xj2, yj2);
						intersections[0][iter] = match1;
						intersections[1][iter] = match2;
						iter++;
					}
				} 
				else if ( yi1 == yi2 ) { 
					match1 = yi1;
					match2 = xj1;
					if (between(match1, yj1, yj2) && between(match2, xi1, xi2)) {
						printf("match! (%d,%d) (%d,%d) -> (%d,%d)\t (%d,%d) -> (%d,%d)\n", match1, match2, xi1, yi1, xi2, yi2, xj1, yj1, xj2, yj2);
						intersections[0][iter] = match2;
						intersections[1][iter] = match1;
						iter++;
					}
				}
			}
		}
	}
	return intersections;
}


int main(int argc, char* argv[]) {
	char inputLine[1500];
	char sep = ',';
	int **carrier[2];
	int carrierPos = 0;

	FILE *ifp = fopen("./03_input.txt", "r");
	if (ifp == NULL) exit(1);

	while(!feof(ifp)) {
		fscanf(ifp, "%s\n,", inputLine);
		int count = 0; // amount of commas
		char *tmp = inputLine;
		while (*tmp) {
			if (sep == *tmp) {
				count++;
			}
			tmp++;
		}
		count++;

		int **array = malloc(2 * sizeof(int*)); // x = array[0], y = array[1]
		array[0] = malloc(count * sizeof(int));
		memset(array[0], 0, count * sizeof(int));
		array[1] = malloc(count * sizeof(int));
		memset(array[1], 0, count * sizeof(int));

		int i=0, increment = 0;
		char direction;
		char *inPtr = inputLine;
		while(sscanf(inPtr, "%c%d,", &direction, &increment) == 2) {
			int lastPosition = i == 0 ? 0 : i-1;
			int YPosition = array[1][lastPosition];
			int XPosition = array[0][lastPosition];
			printf("lastPos: %d,\t dir: %c, incr: %d,\t ypos: %d\t xpos: %d\n", lastPosition, direction, increment,YPosition, XPosition);
			switch(direction) {
				case 'U' :
					array[1][i] = YPosition + increment;
					array[0][i] = XPosition;
					break;
				case 'D' :
					array[1][i] = YPosition - increment;
					array[0][i] = XPosition;
					break;
				case 'R' :
					array[0][i] = XPosition + increment;
					array[1][i] = YPosition;
					break;
				case 'L' :
					array[0][i] = XPosition - increment;
					array[1][i] = YPosition;
					break;
				default :
					fprintf(stderr, "invalid direction %c!\n", direction);

			}
			printf("Array[0][%d]:Array[1][%d] = %d:%d\n", i, i, array[0][i], array[1][i]);
			inPtr += getAmountOfChars(increment) + 2;
			if(i == count) break; // sscanf messes up at the end of the line sometimes ;v
			i++;
		}
		carrier[carrierPos] = array;
		printf("carrierpos = %d\n", carrierPos);
		carrierPos++;
	}

	int **intersections = findIntersections(carrier);
	int shortestDistance = abs(intersections[0][0]) + abs(intersections[1][0]);
	for (int i = 0; i < 100; i++) {
		int distance = abs(intersections[0][i]) + abs(intersections[1][i]);
		if (distance < shortestDistance && distance != 0)
			shortestDistance = distance;
	}

	printf("shortest distance: %d\n", shortestDistance);
	fclose(ifp);

	free(intersections[0]);
	free(intersections[1]);
	free(intersections);
	free(carrier[0][0]);
	free(carrier[0][1]);
	free(carrier[0]);
	free(carrier[1][0]);
	free(carrier[1][1]);
	free(carrier[1]);

}
