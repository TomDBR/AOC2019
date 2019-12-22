#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node {
	char *key;
	struct node *parent;
	struct node **satellites;
	int orbits;
};

struct node *create_node(char *key) {
	struct node *obj = malloc(sizeof(struct node));
	memset(obj, 0, sizeof(struct node));
	if (obj == NULL) return NULL;
	obj->key = strdup(key);
	obj->parent = NULL;
	obj->satellites = NULL;
	obj->orbits = 0;
	return obj;
}

int stringEquals(char *string1, char *string2) {
	for(int i = 0; string1[i] != '\0'; i++) {
		char one = string1[i];
		char two = string2[i];
		if ((int) one != (int) two) return 0;
	}
	return 1;
}

char *splitString(char *line, char *mem) {
	char *memPtr = mem;
	memset(mem, '\0', 8);
	strncpy(memPtr, line, 3);
	strncpy(memPtr+4, line+4, 3);
	return memPtr;
}

int growNodeStruct(struct node ***array, int *oldSize, char operator) {
	if (operator == '+') {
		int newSize = *oldSize + 1;
		if (*array == NULL) {
			if((*array = malloc(1 * sizeof(struct node*))) == NULL) return 0;
		} else {
			if ((*array = (struct node **) realloc(*array, newSize * sizeof(struct node *))) == NULL) return 0;
		}
		memset(*array+*oldSize, 0, sizeof(struct node*));
		*oldSize = newSize;
		return 1;
	} else if (operator == '-') {
		int newSize = *oldSize - 1;
		if (*array == NULL) return 0;
		if ((*array = (struct node **) realloc(*array, newSize * sizeof(struct node *))) == NULL) return 0;
		*oldSize = newSize;
		return 1;
	} else fprintf(stderr, "invalid character!");
	return 0;
}

int addSatellite(struct node *object, struct node *satellite) {
	if (!growNodeStruct(&(object->satellites), &(object->orbits), '+')) {
		printf("growing struct unsuccesful!\n");
		return 0;
	}
	object->satellites[object->orbits-1] = satellite;
	return 1;
}

int addParent(struct node *orbit, struct node *parent) {
	if (orbit->parent == NULL) orbit->parent = parent;
	else return 0;
	return 1;
}

struct node *findInArray(struct node **array, char *key, int count) {
	if (array == NULL) return NULL;
	for (int i = 0; i < count; i++) {
		if (stringEquals(array[i]->key, key)) {
			return array[i];
		}
	}
	return NULL;
}

struct node *findNode(struct node *object, char *key) {
	if (stringEquals(object->key, key)) return object;
	for(int i = 0; i < object->orbits; i++) {
		struct node *child = findNode(object->satellites[i], key);
		if (child) return child;
	}
	return NULL;
}

int orbits(struct node *object, int sofar) {
	int count = sofar ? sofar : 0;
	for (int i = 0; i < object->orbits; i++) {
		count += orbits(object->satellites[i], sofar+1);
	}
	return count;
}

void iterate(struct node **array, int count) {
	for(int i = 0; i < count; i++) {
		struct node *tmp = array[i];
		printf("node: %p: key: %s\t", tmp, tmp->key);
		if(tmp->parent != NULL) printf("is parent: %s\t", tmp->parent->key);
		printf("has in orbit: ");
		for(int j = 0; j < tmp->orbits; j++) {
			printf("%s, ", tmp->satellites[j]->key);
		}
		printf("\n");
	}
}

void findAmountOfTransfers(struct node **array, struct node *obj1, struct node *obj2) {
	int found = 0;
	int cnt = 0;
	struct node *lastVisited;
	struct node *currentNode = obj1;
	while (!found) {
		lastVisited = currentNode;
		currentNode = currentNode->parent;
		if (currentNode == NULL) break; // should never get to NULL 
		//printf("%s = %s\n", lastVisited->key, currentNode->key);
		for (int i = 0; i < currentNode->orbits; i++) {
			// very inefficient!
			if (currentNode->satellites[i] != lastVisited) {
				//printf("++ %s = %s\n", currentNode->key, currentNode->satellites[i]->key);
				if(findNode(currentNode->satellites[i], obj2->key) != NULL) {
					currentNode = currentNode->satellites[i]; // make sure we can use this value in next while loop
					found = 1;
				}
			}
		}
		if(!found) cnt++;
	}
	while (found) {
		lastVisited = currentNode;
		if (stringEquals(currentNode->key, obj2->key)) break;
		for (int i = 0; i < currentNode->orbits; i++) {
			if (findNode(currentNode->satellites[i], obj2->key)) {
				currentNode = currentNode->satellites[i];
				cnt++;
				break;
			}
		}
		if ( lastVisited == currentNode ) found = 0;
	}
	printf("Amount of orbital transfers: %d\n", cnt);
}

int main(int argc, char* argv[]) {
	char *filename = "./06_input.txt";
	FILE *file = fopen(filename, "r");
	char curLine[10];
	struct node **array = NULL;
	int arrCnt = 0;
	if (file == NULL) {
		fprintf(stderr, "Could not open file: %s, exiting.\n", filename);
		exit(0);
	}

	while(fgets(curLine, 9, file)) { // 9 character {key} ) {key} \n \0 , key = 3 chars
		char objectKeys[8];
		char *linePtr = curLine;
		char *ptr = splitString(linePtr, objectKeys);
		struct node *center = findInArray(array, ptr, arrCnt);
		struct node *orbit = findInArray(array, ptr+4, arrCnt);

		if (center == NULL) {
			growNodeStruct(&array, &arrCnt, '+');
			array[arrCnt-1] = create_node(ptr);
			center = array[arrCnt-1];
		} 
		if (orbit == NULL) {
			growNodeStruct(&array, &arrCnt, '+');
			array[arrCnt-1] = create_node(ptr+4);
			orbit = array[arrCnt-1];
		}
		addSatellite(center, orbit);
		addParent(orbit, center);
	}

	fclose(file);
	printf("Total number of direct and indirect orbits is: %d\n", orbits(findInArray(array, "COM", arrCnt), 0));

	struct node *you = findInArray(array, "YOU", arrCnt);
	struct node *san = findInArray(array, "SAN", arrCnt);
	findAmountOfTransfers(array, you, san);
	return 0;
}
