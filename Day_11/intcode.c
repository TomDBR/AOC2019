#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "intcode.h"

long *array = NULL;
int arraySize = 0;
static int verbosity;
static char *fileName;

int receiveNumber() { int number = -1;
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

void loadData() 
{
	FILE *file = fopen(fileName, "r");
	if(file == NULL) exit(1);
	int curIntcode = 0;
	while(fscanf(file, "%d,", &curIntcode) == 1) {
		array = realloc(array, ++arraySize * sizeof(long));
		array[arraySize-1] = curIntcode;
	}
	fclose(file);
}

int getAmountOfChars(int input)
{
	char tmpstr[10], *ptr = tmpstr;
	snprintf(tmpstr, 10, "%d", input);
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
	char tmpStr[length], *ptr;
	snprintf(tmpStr, length+1, "%d", instructionPtr);
	if(tmpStr[length-2] == '0') 
		ptr = &tmpStr[length-1];
	else ptr = &tmpStr[length-2];
	return(atoi(ptr));
}

int getParameterMode(int argNr, int instruction)
{
	int len = getAmountOfChars(instruction), opcodeSize = 2; //opcode is always 2 digits
	char tmp[len]; snprintf(tmp, len+1, "%d", instruction);
	int posInString = len - opcodeSize - argNr;
	if (posInString < 0)  return 0;
	return (int) tmp[posInString] - 48; // convert char back to int
}

int getArgument(int instructionPointer, int parameterMode, int relativeBase) 
{
	switch (parameterMode) {
		case 0: // POSITION MODE
			if (array[instructionPointer] > arraySize) {
				int tmp = arraySize;
				arraySize = array[instructionPointer]+1;
				array = realloc(array, arraySize * sizeof(long));
				long *ptr = &array[tmp]; memset(ptr, 0, (arraySize-tmp) * sizeof(long));
			}
			return array[instructionPointer];
		case 1: // IMMEDIATE MODE
			return instructionPointer;
		case 2: // RELATIVE MODE
			return relativeBase + array[instructionPointer];
		default: // what?
			fprintf(stderr, "Invalid parameter mode %d!\n", parameterMode);
	}
	return -1;
}

void doFunction()
{
	int relativeBase = 0;
	int instructionPtr = 0; // the value in the array we are currently at
	int instruction = 0; // the instruction we read in the array, at position [instructionPtr]
	int opcode = 0; // opcode is stored in [instruction]
	while(opcode != 99) {
		instruction = array[instructionPtr];
		opcode = getOpcode(instruction);
		int arg1 = getArgument(instructionPtr+1, getParameterMode(1, instruction), relativeBase);
		int arg2 = getArgument(instructionPtr+2, getParameterMode(2, instruction), relativeBase);
		int arg3 = getArgument(instructionPtr+3, getParameterMode(3, instruction), relativeBase);
		switch (opcode) {
			case 1 :
				if (verbosity) fprintf(stderr, "case 1:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				array[arg3] = array[arg1] + array[arg2];
				instructionPtr+=4;
				break;
			case 2 :
				if (verbosity) fprintf(stderr, "case 2:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				array[arg3] = array[arg1] * array[arg2];
				instructionPtr+=4;
				break;
			case 3 :
				if (verbosity) fprintf(stderr, "\ncase 3:\tinstruction: %d,%d\nopcode 3 expects numeric input: \n", instruction, arg1);
				array[arg1] = receiveNumber();
				instructionPtr+=2;
				break;
			case 4 :
				if (verbosity) fprintf(stderr, "case 4:\tinstruction: %d,%d\nopcode 4 output: %ld\n", instruction, arg1, array[arg1]);
				printf("%ld\n", array[arg1]);
				fflush(stdout);
				instructionPtr+=2;
				break;
			case 5 : // jump if true
				if (verbosity) fprintf(stderr, "case 5:\tinstruction: %d,%d,%d\n", instruction, arg1, arg2);
				if (array[arg1] != 0)
					instructionPtr = array[arg2];
				else
					instructionPtr+=3;
				break;
			case 6 : // jump if false
				if (verbosity) fprintf(stderr, "case 6:\tinstruction: %d,%d,%d\n", instruction, arg1, arg2);
				if (array[arg1] == 0)
					instructionPtr = array[arg2];
				else
					instructionPtr+=3;
				break;
			case 7 : // less than
				if (verbosity) fprintf(stderr, "case 7:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				if (array[arg1] < array[arg2]) 
					array[arg3] = 1;
				else
					array[arg3] = 0;
				instructionPtr+=4;
				break;
			case 8 : // equals
				if (verbosity) fprintf(stderr, "case 8:\tinstruction: %d,%d,%d,%d\n", instruction, arg1, arg2, arg3);
				if (array[arg1] == array[arg2]) 
					array[arg3] = 1;
				else
					array[arg3] = 0;
				instructionPtr+=4;
				break;
			case 9 : // adjust relative base by value of its only parameter
				if (verbosity) fprintf(stderr, "case 9:\tinstruction: %d,%d\n", instruction, arg1);
				relativeBase += array[arg1];
				instructionPtr+=2;
				break;
			case 99 : 
				if (verbosity) fprintf(stderr, "case 99: halting program: %d\n", instruction);
				instructionPtr+=4;
				break;
			default:
				fprintf(stderr, "invalid opcode: %d\n", opcode);
				break;
		}
	}
}

int main() {
	fileName = "./input.txt";
	verbosity = 0;

	loadData();
	doFunction();
	return 0;
}
