#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../UTILS/shell_cmd.h"
#include "../UTILS/util.h"

#define UP 	(1)
#define DOWN 	(2)
#define LEFT 	(3)
#define RIGHT 	(4)

#define BLACK   (0)
#define WHITE   (1)

static int verbosity = 0;
int map_size = 0;
struct coord **map = NULL;

struct coord { 
	int X;
	int Y;
	int color;
};

typedef struct boundaries {
	int x_min;
	int y_min;
	int x_max;
	int y_max;
} b_t;

int receive_engine_code(FILE *f)
{
	int code = -1;
	if (fscanf(f, "%d", &code) != 1) fclose(f);
	return code;
}

void send_engine_code(process_t p, int code) 
{
	dprintf(p.fd_read, "%d\n", code);
	fsync(p.fd_read);
}

void changeDirection(int *dir, int degrees, int *xCoord, int *yCoord) 
{
	// degrees -> 0:turn left 90 degrees, 1: turn RIGHT 90 degrees
	int tmp = *dir;	
	switch (tmp) { // get new direction
		case UP: 
			*dir = (degrees) ? RIGHT : LEFT;
			break;
		case DOWN:
			*dir = (degrees) ? LEFT : RIGHT;
			break;
		case LEFT:
			*dir = (degrees) ? UP : DOWN;
			break;
		case RIGHT:
			*dir = (degrees) ? DOWN : UP;
			break;
	}
	switch (*dir) { // change coords accordingly
		case UP: 
			*yCoord += 1;
			break;
		case DOWN:
			*yCoord -= 1;
			break;
		case LEFT:
			*xCoord -= 1;
			break;
		case RIGHT:
			*xCoord += 1;
			break;
	}
}

struct coord *getCoord(int x, int y) 
{
	for (int i = 0; i < map_size; i++) {
		if (map[i]->X == x && map[i]->Y == y) 
			return map[i];
	}
	return NULL;
}

void addToMap(struct coord *coord) 
{
		map_size += 1;
		map = realloc(map, map_size * sizeof(struct coord *));
		map[map_size-1] = coord;
		if (verbosity) printf("adding coord (%p) to map, size is now %d\n", (void *) coord, map_size);
}

struct coord *createCoord(int x, int y, int color) 
{
	struct coord *coord = getCoord(x, y);
	if (coord == NULL) {
		coord = malloc(sizeof(struct coord));
		coord->X = x;
		coord->Y = y;
		coord->color = color;
		if (verbosity) printf("new coord:\t (%d,%d) -> color: %s\n", x, y, color ? "WHITE" : "BLACK");
		addToMap(coord);
	}
	return coord;
}

void checkBoundaries(b_t *b, int x, int y) 
{
	if (x < b->x_min) b->x_min = x;
	else if (x > b->x_max) b->x_max = x;
	if (y < b->y_min) b->y_min = y;
	else if (y > b->y_max) b->y_max = y;
}

void runProg(b_t *b, int startColor) 
{
	char *cmd[] = { "intcode", "./input.txt", "-1", "0", NULL};
	process_t intcodePrg = process(cmd);
	FILE *f = fdopen(intcodePrg.fd_write, "r");
	if (!f) die("failed to open fd\n");


	int dir = UP, xCoord = 0, yCoord = 0;
	struct coord *lastCoord = createCoord(xCoord, yCoord, startColor);
	while (1) {
		send_engine_code(intcodePrg, lastCoord->color);
		int color = receive_engine_code(f); 
		int degrees = receive_engine_code(f);
		if (color == -1 || degrees == -1) break;
		if (verbosity) printf("lastCoord: %p, color: %s, degrees: %s\n", (void *) lastCoord, color ? "WHITE" : "BLACK", degrees ? "+90" : "-90");
		lastCoord->color = color;
		changeDirection(&dir, degrees, &xCoord, &yCoord);
		if (b != NULL) checkBoundaries(b, xCoord, yCoord);
		lastCoord = createCoord(xCoord, yCoord, BLACK);
	}
	wait(NULL);
}

void cleanup() 
{
	for (int i = 0; i < map_size; i++)
		free(map[i]);
	free(map);
	map = NULL;
	map_size = 0;
}

int main() 
{
	runProg(NULL, BLACK);
	printf("part 1: %d\n", map_size);
	cleanup();

	printf("part 2: \n");
	b_t b = { 0, 0, 0, 0};
	runProg(&b, WHITE);
	for (int y = b.y_max; y >= b.y_min; y--) {
		for (int x = b.x_min; x <= b.x_max; x++) {
			struct coord *tmp;
			if ((tmp = getCoord(x, y)) != NULL) {
				if (tmp->color == WHITE) printf("█");
				else printf(" ");
			} else printf(" ");
		}
		printf("\n");
	}
	if (verbosity) printf("X: %d->%d\tY: %d->%d\n", b.x_min, b.x_max, b.y_min, b.y_max);
	cleanup();
	return 0;
}
