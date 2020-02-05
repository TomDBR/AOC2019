#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static int engNo;
static char *fileName;

int receiveNumber() 
{
	int number = -1;
	int retVal = 0 ;
	while(retVal != 1) {
		retVal = fscanf(stdin, "%d", &number);
		if (retVal == 0) {
			fprintf(stderr, "Input needs to be numeric!\n");
			while(fgetc(stdin) != '\n');
		}
	}
	return number;
}

int *loadData() 
{
	FILE *file = fopen(fileName, "r");
	if(file == NULL) exit(1);
	int curIntcode = 0;
	int *array = calloc(1, sizeof(int));
	int arraySize = 0;

	while(fscanf(file, "%d,", &curIntcode) == 1) {
		if(arraySize) array = (int *) realloc(array, (arraySize+1)*sizeof(int));
		if(array != NULL) *(array+arraySize) = curIntcode;
		arraySize++;
	}
	fclose(file);
	return array;
}

int getAmountOfChars(int input)
{
	char tmpstr[10];
	snprintf(tmpstr, 10, "%d", input);
	char *ptr = tmpstr;
	int count = 0;
	while (*ptr) {
		if(*ptr != '\0') count++;
		ptr++;
	}
	return count;
}

int getOpcode(int instructionPtr)
{
	int length = getAmountOfChars(instructionPtr);
	if (length == 1 || length == 2) return instructionPtr;
	char tmpStr[length];
	snprintf(tmpStr, length+1, "%d", instructionPtr);
	char *ptr;
	if(tmpStr[length-2] == '0') ptr = &tmpStr[length-1];
	else ptr = &tmpStr[length-2];
	return(atoi(ptr));
}

int getValForParamMode(int argNr, int instructionPtr)
{
	int length = getAmountOfChars(instructionPtr);
	char tmpStr[length];
	snprintf(tmpStr, length+1, "%d", instructionPtr);
	if ((length - 2 - argNr) < 0 ) return 0;
	int paramMode = (int) tmpStr[length-2-argNr] - 48;
	//printf("instrPtr: %d, tmpStr: %s, ptr: %d, argNr = %d, length = %d\n", instructionPtr, tmpStr, paramMode, argNr, length);
	return paramMode;
}

int doFunction(int *array, int verbosity)
{
	int output = 0;
	int instructionPtr = 0; // the value in the array we are currently at
	int instruction = 0; // the instruction we read in the array, at position [instructionPtr]
	int opcode = 0; // opcode is stored in [instruction]
	while(opcode != 99) {
		instruction = array[instructionPtr];
		opcode = getOpcode(instruction);
		// if params 0 -> read value in array at index of instructionpointer, and then get the value in array at that position
		// if params 1 -> read value in array at index of instrPtr and use that value
		int paramArg1 = getValForParamMode(1, instruction);
		int paramArg2 = getValForParamMode(2, instruction);
		int paramArg3 = getValForParamMode(3, instruction);
		int arg1 = paramArg1 ? instructionPtr+1 : array[instructionPtr+1];
		int arg2 = paramArg2 ? instructionPtr+2 : array[instructionPtr+2];
		int arg3 = paramArg3 ? instructionPtr+3 : array[instructionPtr+3]; // mainly used for location
		if (verbosity) printf("modes -> param1: %d, param2: %d, param3: %d\t", paramArg1, paramArg2, paramArg3);
		switch (opcode) {
			case 1 :
				if (verbosity) printf("case 1:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				array[arg3] = array[arg1] + array[arg2];
				instructionPtr+=4;
				break;
			case 2 :
				if (verbosity) printf("case 2:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				array[arg3] = array[arg1] * array[arg2];
				instructionPtr+=4;
				break;
			case 3 :
				if (verbosity) printf("\ncase 3:\tinstruction: %d,%d\n", instruction, arg1);
				if (verbosity) printf("opcode 3 expects numeric input: \n");
				array[arg1] = receiveNumber();
				instructionPtr+=2;
				if (engNo != -1) fprintf(stderr, "%d: ENGINE %d\t case 3:\tinstruction: %d,%d\t %d\n", getpid(), engNo, instruction, arg1, array[arg1]);
				break;
			case 4 :
				if (verbosity) printf("case 4:\tinstruction: %d,%d\n", instruction, arg1);
				if (verbosity) printf("opcode 4 output: %d\n", array[arg1]);
				if (engNo != -1) fprintf(stderr, "%d: ENGINE %d\t opcode 4 output: %d\n", getpid(), engNo, array[arg1]);
				printf("%d\n", array[arg1]);
				fflush(stdout);
				output = array[arg1];
				instructionPtr+=2;
				break;
			case 5 : // jump if true
				if (verbosity) printf("case 5:\tinstruction: %d,%d,%d\n", instruction, arg1, arg2);
				if (array[arg1] != 0)
					instructionPtr = array[arg2];
				else
					instructionPtr+=3;
				break;
			case 6 : // jump if false
				if (verbosity) printf("case 6:\tinstruction: %d,%d,%d\n", instruction, arg1, arg2);
				if (array[arg1] == 0)
					instructionPtr = array[arg2];
				else
					instructionPtr+=3;
				break;
			case 7 : // less than
				if (verbosity) printf("case 7:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				if (array[arg1] < array[arg2]) 
					array[arg3] = 1;
				else
					array[arg3] = 0;
				instructionPtr+=4;
				break;
			case 8 : // equals
				if (verbosity) printf("case 8:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				if (array[arg1] == array[arg2]) 
					array[arg3] = 1;
				else
					array[arg3] = 0;
				instructionPtr+=4;
				break;
			case 99 : 
				if (verbosity) printf("case 99: halting program: %d\n", instruction);
				if (engNo != -1) fprintf(stderr, "ENGINE NO: %d\t%d\n", engNo, output);

				//printf("%d\n", output);
				fprintf(stderr, "%d\n", output);
				fflush(stdout);
				instructionPtr+=4;
				break;
			default:
				fprintf(stderr, "%d: ENGINE %d\t invalid opcode: %d\n", getpid(), engNo, opcode);
				return -1;
		}
	}
	return array[0];
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Error: 2 arguments expected!\n");
		fprintf(stderr, "Usage: intCodeComputer {engineNumber} {verbosity}\n");
		fprintf(stderr, "{Engine Number} : any number\n");
		fprintf(stderr, "{verbosity} : [0,1]\n");
		exit(1);
	}
	fileName = "../07_input.txt";
	engNo = atoi(argv[1]);
	int verbosity = atoi(argv[2]);

	int *array = loadData();
	printf("%d\n", doFunction(array, verbosity));

	free(array);
	return 0;
}
