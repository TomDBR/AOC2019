#include <stdio.h>
#include <stdlib.h>

int doFunction(int *array) {
	int position = 0;
	int opcode = 0;
	while(opcode != 99) {
		opcode = array[position];
		int argOne = array[position+1];
		int argTwo = array[position+2];
		int location = array[position+3];
		switch (opcode) {
			case 1 :
				array[location] = array[argOne] + array[argTwo];
				break;
			case 2 :
				array[location] = array[argOne] * array[argTwo];
				break;
			case 99 : 
				break;
			default:
				fprintf(stderr, "invalid opcode: %d!!1!1\n", opcode);
		}
		position+=4;
	}
	return array[0];

}
int main(int argc, char* argv[]) {
	int magicNumber = 19690720;
	int array[] = { 
	#include "./02_input.txt" 
	};
	array[1] = 12;
	array[2] = 2;
	printf("Value at position 0 in array: %d.\n", doFunction(array));

	for(int i = 0; i < 100; i++) {
		for(int x = 0; x < 100; x++) {
			int array[] = { 
			#include "./02_input.txt"
			};
			array[1] = i;
			array[2] = x;
			int numAtPosZero = doFunction(array);
			if (numAtPosZero == magicNumber) {
				printf("Position 1: %d,\tposition 2: %d\n", array[1], array[2]);
				return 0;
			}
		}
	}
	return 0;
}

