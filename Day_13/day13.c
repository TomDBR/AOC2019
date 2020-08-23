#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include "../UTILS/shell_cmd.h"
#include "../UTILS/util.h"
#include <poll.h>
#include <string.h>

int verbosity = 0;
struct output **tiles = NULL;
int tiles_size = 0;
char *cmd[] = { "intcode", "./input.txt", "-1", "0", NULL };
int width = 0, height = 0;

struct output {
	int x;
	int y;
	int type;
};


void output_to_string(struct output *tile)
{
	printf("TILE:\t(%d,%d) -> TYPE: %d\n", tile->x, tile->y, tile->type);
}

struct output *find_type(int type)
{
	for (int i = 0; i < tiles_size; i++) 
		if (tiles[i]->type == type) return tiles[i];
	return NULL;
}

int findTile(int x, int y)
{
	for (int i = 0; i < tiles_size; i++) {
		struct output *t = tiles[i];
		if (t->x == x && t->y == y) return i;
	}
	return -1;
}

// create new tile if it doesn't exist, otherwise update its type
struct output *assignTile(int x, int y, int type)
{
	//printf("Tiles size is: %d (%d-%d)\n", tiles_size, width, height);
	int i = 0;
	if ((i = findTile(x, y)) != -1) {
		tiles[i]->type = type;
		return tiles[i];
	}
	tiles = realloc(tiles, ++tiles_size * sizeof(struct output *));
	tiles[tiles_size - 1] = malloc(sizeof(struct output));
	tiles[tiles_size - 1]->x = x;
	tiles[tiles_size - 1]->y = y;
	tiles[tiles_size - 1]->type = type;
	if (x > width) width = x;
	if (y > height) height = y;
	return tiles[tiles_size -1];
}

char get_block_for_type(int type) 
{
	char block;
	switch (type) {
		case 0: // empty tile
			block = ' '; break;
		case 1: // wall tile
			block = '|'; break;
		case 2: // block tile
			block = '#'; break;
		case 3: // horizontal paddle
			block = '-'; break;
		case 4: // ball tile
			block = 'o'; break; }
	return block;
}

int isScore(int x, int y)
{
	return (x == -1 && y == 0) ? 1 : 0;
}

void query_machine(int fd, int *x, int *y, int *type) {
	char buf[20], c;
	bzero(buf, 20);
	int newlineCnt = 0, bufIdx = 0;
	while (newlineCnt < 3 && read(fd, &c, 1) == 1) {
		if (c == '\n') {
			switch (newlineCnt) {
				case 0:
					*x = atoi(buf); break;
				case 1:
					*y = atoi(buf); break;
				case 2:
					*type = atoi(buf); break;
			}
			newlineCnt++;
			bufIdx = 0;
			bzero(buf, 20);
		} else {
			buf[bufIdx++] = c;
		}
	}
	if (verbosity) printf("(%d,%d) => TYPE %d\n", *x, *y, *type);
}

int get_move() {
	int ret = 0;
	struct output *ball = find_type(4);
	struct output *paddle = find_type(3);
	if (!ball || !paddle) {
		die("The ball and/or paddle disappeared!");
	}
	if (paddle->x < ball->x) ret = 1;
	if (paddle->x > ball->x) ret = -1;
	return ret;
}

int main() 
{
	process_t machine = process(cmd);
	// use poll to query whether there's any data to be read before using scanf
	struct pollfd poll_f = { machine.fd_write, POLLIN, 0 };
	int x, y, type; // used to save the output of the intcode program in

	while (1) {
		if (poll(&poll_f, 1, 100) > 0) {
			if (poll_f.revents & POLLIN) {
				query_machine(machine.fd_write, &x, &y, &type);
				if (isScore(x, y)) // when this happens, the program is done drawing and starts accepting input, initial score 0
					break;
				assignTile(x, y, type);
			}
		}
	}

	// check amount of block tiles
	int count = 0;
	for (int i = 0; i < tiles_size; i++) {
		if (tiles[i]->type == 2) count++;
	}
	printf("PART 1:\tAmount of block tiles: %d\n", count);

	// PART 2: PLAY THE GAME
	while((waitpid(machine.pid, NULL, WNOHANG)) == 0) {
		dprintf(machine.fd_read, "%d\n", get_move());
		while (1) {
			if (poll(&poll_f, 1, 10) <= 0) 
				break;
			if (poll_f.revents & POLLHUP) 
				break;
			if (poll_f.revents & POLLIN) {
				query_machine(machine.fd_write, &x, &y, &type);
				if (isScore(x, y)) 
					printf("score: %d\n", type);
				else
					assignTile(x, y, type);
			} 
		}
	}
	wait(NULL);
	
	printf("PART 2:\tScore after playing game is: %d\n", type);
	for (int i = 0; i < tiles_size; i++)
		free(tiles[i]);
	free(tiles);
	return 0;
}
