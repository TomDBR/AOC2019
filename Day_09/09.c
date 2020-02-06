#include <unistd.h>
#include <stdio.h>
#include "../UTILS/shell_cmd.h"
#include "../UTILS/util.h"

int main() {
	long keycode = 0; FILE *f;
	char *p1_cmd[] = { "intcode", "./input.txt", "-1", "0", NULL };

	// PART 1
	process_t p1 = process(p1_cmd);
	dprintf(p1.fd_read, "%d\n", 1); close(p1.fd_read);
	if (!(f = fdopen(p1.fd_write, "r"))) die("Failed to open file descriptor!");
	fscanf(f, "%ld\n", &keycode);
	fclose(f);
	printf("PART I: BOOST keycode: %ld\n", keycode);

	// PART 2
	process_t p2 = process(p1_cmd);
	dprintf(p2.fd_read, "%d\n", 2); close(p2.fd_read);
	if (!(f = fdopen(p2.fd_write, "r"))) die("Failed to open file descriptor!");
	fscanf(f, "%ld\n", &keycode);
	fclose(f);
	printf("PART II: coordinates of distress signal: %li\n", keycode);
	return 0;
}
