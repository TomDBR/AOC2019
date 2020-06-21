#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include "../UTILS/shell_cmd.h"
#include "../UTILS/util.h"
#include <poll.h>

int verbosity = 0;
struct output **tiles = NULL;
int tiles_size = 0;
char *cmd[] = { "intcode", "./input.txt", "-1", "0", NULL };
int width = 0, height = 0;
int score = 0;

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

int isScore(struct output *s)
{
	return (s->x == -1 && s->y == 0) ? 1 : 0;
}

void redraw_screen() 
{
	int max_y = 0, max_x = 0;
	getmaxyx(stdscr, max_y, max_x);
	int x_start = ( max_x / 2 ) - ( width / 2 );
	int y_start = ( max_y / 2 ) - ( height / 2 );

	noecho();
	curs_set(FALSE); // hide cursor
	nocbreak();
	for (int i = 0; i < tiles_size; i++) {
		int x = tiles[i]->x, y = tiles[i]->y, type = tiles[i]->type;
		char block = get_block_for_type(type);
		mvprintw(y_start + y, x_start + x, &block);
		refresh();
	}
}

int query_machine(FILE *f, int *x, int *y, int *type) {
	int ret = fscanf(f, "%d\n%d\n%d", x, y, type);
	if (verbosity) printf("x: %d, y: %d, type: %d\n", *x, *y, *type);
	return ret == 3 ? 1: 0;
}

int get_move() {
	struct output *ball = find_type(4);
	struct output *paddle = find_type(3);
	if (!ball || !paddle) die("something went wrong!");
	if (paddle->x < ball->x) return 1;
	if (paddle->x > ball->x) return -1;
	return 0;
}

int main() 
{
	signal(SIGPIPE, SIG_IGN);
	// Run program
	process_t machine = process(cmd);
	// use poll to query whether there's any data to be read before using scanf
	struct pollfd poll_f = { machine.fd_write, POLL_IN|POLL_PRI, 0 };
	FILE *f;
	int x, y, type; // used to save the output of the intcode program in
	int max_y = 0, max_x = 0; // used to determine size of screen for ncurses

 	if (!(f = fdopen(machine.fd_write, "r"))) 
		die("Failed to open file descriptor!\n");

	while (query_machine(f, &x, &y, &type)) {
		if (x == -1 && y == 0) // when this happens, the program is done drawing and starts accepting input, initial score 0
			break;
		assignTile(x, y, type);
	}

	// check amount of block tiles
	int count = 0;
	for (int i = 0; i < tiles_size; i++) {
		if (tiles[i]->type == 2) count++;
	}
	printf("PART 1:\tAmount of block tiles: %d\n", count);

	// PART 2: PLAY THE GAME
	// Draw a screen
	initscr();
	noecho();
	curs_set(FALSE); // hide cursor
	cbreak();

	// center the screen
	getmaxyx(stdscr, max_y, max_x);
	redraw_screen();

	while((waitpid(machine.pid, NULL, WNOHANG)) == 0) {
		int res = get_move(), score = 0;
		mvprintw(max_y - 1, 0, "MOVING TO THE %s!", abs(res) == 1 ? res == 1 ? "RIGHT": "LEFT" : "NOT");
		mvprintw(max_y - 2, 0, "Current score: %d", score);
		dprintf(machine.fd_read, "%d\n", res);
		for (int i = 0; i < (abs(res) ? 4 : 2); i++) {
			query_machine(f, &x, &y, &type);
			assignTile(x, y, type);
		}
		if (poll(&poll_f, 1, 100)) {
			query_machine(f, &x, &y, &type);
			switch (type) {
				case 0:
					//printf("SCORE: %d, YOU DIED!\n", type);
					mvprintw(max_y - 3, 0, "SCORE: %d, YOU DIED!\n", type);
					break;
				default:
					score = type;
					mvprintw(max_y - 3, 0, "SCORE: %d\n", score);
			}
		}
		redraw_screen();
		usleep(10000);
	}
	usleep(1000000);
	clear();
	endwin();
	wait(NULL);
	return 0;
}
