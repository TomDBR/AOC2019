#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include "../UTILS/shell_cmd.h"
#include "../UTILS/util.h"

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

int findTile(int x, int y)
{
	for (int i = 0; i < tiles_size; i++) {
		struct output *t = tiles[i];
		if (t->x == x && t->y == y) return i;
	}
	return 0;
}

struct output *addTile(int x, int y, int type)
{
	int i = 0;
	if ((i = findTile(x, y))) {
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

char get_block_for_type(int type) {
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

void *update_screen(void *f) {
	noecho();
	curs_set(FALSE); // hide cursor
	cbreak();
	int x, y, type, cnt = 0;
	int max_y = 0, max_x = 0;
	getmaxyx(stdscr, max_y, max_x);
	int x_start = ( max_x / 2 ) - ( width / 2 );
	int y_start = ( max_y / 2 ) - ( height / 2 );
	while (fscanf(f, "%d\n%d\n%d", &x, &y, &type) == 3) {
		cnt++;
		mvprintw(max_y - 3, 0, "in loop: [%d]. x: %d, y: %d, type: %d", cnt, x, y, type);
		struct output *s = addTile(x, y, type);
		if (isScore(s)) {
			score = s->type;
		} else {
			char block = get_block_for_type(s->type);
			mvprintw(y_start + y, x_start + x, &block);
		}
		refresh();
	}
	return NULL;
}

int main() 
{
	signal(SIGPIPE, SIG_IGN);
	// Run program
	process_t machine = process(cmd);
	FILE *f; int x, y, type;
 	if (!(f = fdopen(machine.fd_write, "r"))) 
		die("Failed to open file descriptor!\n");

	while (fscanf(f, "%d\n%d\n%d", &x, &y, &type) == 3) {
		//printf("x: %d, y: %d, type: %d\n", x, y, type);
		if (x == -1 && y == 0) // when this happens, the program is done drawing and starts accepting input, initial score 0
			break;
		addTile(x, y, type);
	}
	//printf("char was '%c'\n", fgetc(f));

	// check amount of block tiles
	int count = 0;
	for (int i = 0; i < tiles_size; i++) {
		if (tiles[i]->type == 2) count++;
	}
	printf("Amount of block tiles: %d\n", count);

	// Draw a screen
	initscr();
	noecho();
	curs_set(FALSE); // hide cursor
	cbreak();

	// center the screen
	int max_y = 0, max_x = 0;
	getmaxyx(stdscr, max_y, max_x);
	int x_start = ( max_x / 2 ) - ( width / 2 );
	int y_start = ( max_y / 2 ) - ( height / 2 );

	// initial layout
	for (int i = 0; i < tiles_size; i++) {
		int x = tiles[i]->x, y = tiles[i]->y, type = tiles[i]->type;
		char block = get_block_for_type(type);
		mvprintw(y_start + y, x_start + x, &block);
	}
	refresh(); // redraw screen!

	//pthread_t inc_x_thread;
	//if(pthread_create(&inc_x_thread, NULL, update_screen, f)) die("error creating thread!");


	// start game loop
	nodelay(stdscr, TRUE); // makes getch non blocking, returns ERR if no key pressed
	int exit_loop = 0; // set to 1 when q is pressed to exit loop
	//int score = 0;
	while (1) {
		if (exit_loop) break;
		int key = getch(), res;
		switch (key) {
			case 108:
				res = 1; break;
			case 113:
				exit_loop = 1; 
				continue;
			case 104:
				res = -1; break;
			default:
				res = 0;
		}
		mvprintw(max_y - 1, 0, "key: %c\t(digit: %d) -> res: %d", (char) key, key, res);
		mvprintw(max_y - 2, 0, "Current score: %d", score);
		dprintf(machine.fd_read, "%d\n", res);
		int x, y, type;
		for (int i = 0; i < (abs(res) ? 4 : 2); i++) {
			fscanf(f, "%d\n%d\n%d", &x, &y, &type);
			mvprintw(max_y - 3, 0, "in loop: [%d]. x: %d, y: %d, type: %d", i, x, y, type);
			struct output *s = addTile(x, y, type);
			if (isScore(s)) {
				score = s->type;
			} else {
				char block = get_block_for_type(s->type);
				mvprintw(y_start + y, x_start + x, &block);
			}
			refresh();
		}
		refresh();
		usleep(200000);
	}
	clear();
	endwin();
	wait(NULL);
	return 0;
}
