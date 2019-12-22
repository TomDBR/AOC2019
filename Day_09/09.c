#include <stdio.h>
#include "intcode.h"

int main(int argc, char *argv[]) {
	printf("PART I: BOOST keycode: %li\n", handleInput("./input.txt", 0));
	printf("PART II: coordinates of distress signal: %li\n", handleInput("./input.txt", 0));
	return 0;
}
