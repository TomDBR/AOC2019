#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "intcode.h"

long *array = NULL;
long arraySize = 0;
static int engNo;
static int verbosity;
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

void loadData() 
{
	FILE *file = fopen(fileName, "r");
	if(file == NULL) exit(1);
	long curIntcode = 0;
	array = calloc(1, sizeof(long));
	arraySize = 0;

	while(fscanf(file, "%ld,", &curIntcode) == 1) {
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

long getArgument(int instructionPointer, int parameterMode, int relativeBase) 
{
	switch (parameterMode) {
		case 0: // POSITION MODE
			if (instructionPointer > arraySize) return -1;
			if (array[instructionPointer] > arraySize) {
				long tmp = arraySize;
				arraySize = array[instructionPointer]+1;
				array = realloc(array, arraySize * sizeof(long));
				long *ptr = &array[tmp]; memset(ptr, 0, (arraySize-tmp-1) * sizeof(long));
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

void getArgs(int opcode, long *arg1, long *arg2, long *arg3, int instruction, int instructionPtr, int relativeBase) {
	*arg1 = 0; *arg2 = 0; *arg3 = 0;
	if (1 <= opcode && opcode <= 9) {
		*arg1 = getArgument(instructionPtr+1, getParameterMode(1, instruction), relativeBase);
		if (opcode == 1 || opcode == 2 || opcode == 7 || opcode == 8) 
			*arg3 = getArgument(instructionPtr+3, getParameterMode(3, instruction), relativeBase);
		if (opcode != 3 || opcode != 4) 
			*arg2 = getArgument(instructionPtr+2, getParameterMode(2, instruction), relativeBase);
	}
}

long doFunction()
{
	long output = 0;
	int relativeBase = 0;
	int instructionPtr = 0; // the value in the array we are currently at
	int instruction = 0; // the instruction we read in the array, at position [instructionPtr]
	int opcode = 0; // opcode is stored in [instruction]
	while(opcode != 99) {
		instruction = array[instructionPtr];
		opcode = getOpcode(instruction);
		long arg1, arg2, arg3; 	getArgs(opcode, &arg1, &arg2, &arg3, instruction, instructionPtr, relativeBase);
		if (verbosity) fprintf(stderr, "instruction: %d, opcode: %d, arg1: %ld, arg2: %ld, arg3: %ld\n", instruction, opcode, arg1, arg2, arg3);
		switch (opcode) {
			case 1 :
				if (verbosity) fprintf(stderr, "case 1:\tinstruction: %d,%ld,%ld,%ld\n", instruction, arg1, arg2, arg3);
				array[arg3] = array[arg1] + array[arg2];
				instructionPtr+=4;
				break;
			case 2 :
				if (verbosity) fprintf(stderr, "case 2:\tinstruction: %d, %ld = %ld * %ld\n", instruction, array[arg3], array[arg1], array[arg2]);
				array[arg3] = array[arg1] * array[arg2];
				instructionPtr+=4;
				break;
			case 3 :
				if (engNo != -1) fprintf(stderr, "%d: ENGINE %d: ", getpid(), engNo);
				if (verbosity) fprintf(stderr, "\ncase 3:\tinstruction: %d,%ld\nopcode 3 expects numeric input: \n", instruction, arg1);
				array[arg1] = receiveNumber();
				instructionPtr+=2;
				break;
			case 4 :
				if (engNo != -1) fprintf(stderr, "%d: ENGINE %d:", getpid(), engNo);
				if (verbosity) fprintf(stderr, "case 4:\tinstruction: %d,%ld\nopcode 4 output: %ld\n", instruction, arg1, array[arg1]);
				output = array[arg1];
				printf("%ld\n", output);
				fflush(stdout);
				instructionPtr+=2;
				break;
			case 5 : // jump if true
				if (verbosity) fprintf(stderr, "case 5:\tinstruction: %d,%ld,%ld\n", instruction, arg1, arg2);
				if (array[arg1] != 0)
					instructionPtr = array[arg2];
				else
					instructionPtr+=3;
				break;
			case 6 : // jump if false
				if (verbosity) fprintf(stderr, "case 6:\tinstruction: %d,%ld,%ld\n", instruction, arg1, arg2);
				if (array[arg1] == 0)
					instructionPtr = array[arg2];
				else
					instructionPtr+=3;
				break;
			case 7 : // less than
				if (verbosity) fprintf(stderr, "case 7:\tinstruction: %d,%ld,%ld,%ld\n", instruction, arg1, arg2, arg3);
				if (array[arg1] < array[arg2]) {
					array[arg3] = 1;
				} else {
					array[arg3] = 0;
				}
				instructionPtr+=4;
				break;
			case 8 : // equals
				if (verbosity) fprintf(stderr, "case 8:\tinstruction: %d,%ld,%ld,%ld\n", instruction, arg1, arg2, arg3);
				if (array[arg1] == array[arg2]) {
					array[arg3] = 1;
				} else {
					array[arg3] = 0;
				}
				instructionPtr+=4;
				break;
			case 9 : // adjust relative base by value of its only parameter
				if (verbosity) fprintf(stderr, "case 9:\tinstruction: %d,%ld\n", instruction, arg1);
				relativeBase += array[arg1];
				instructionPtr+=2;
				break;
			case 99 : 
				if (verbosity) fprintf(stderr, "case 99: halting program: %d\n", instruction);
				if (engNo != -1) fprintf(stderr, "ENGINE NO: %d\t%ld\n", engNo, output);
				break;
			default:
				if (engNo != -1) fprintf(stderr, "%d: ENGINE %d\t: ", getpid(), engNo);
				fprintf(stderr, "invalid opcode: %d\n", opcode);
				return -1;
		}
	}
	return output;
}

int main(int argc, char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "Error: 3 arguments expected!\n");
		fprintf(stderr, "Usage: ./intcode {fileName} {engineNumber} {verbosity}\n");
		fprintf(stderr, "\tengineNumber: any number\n");
		fprintf(stderr, "\tverbosity: [0,1]\n");
		exit(1);
	}
	fileName = argv[1];
	engNo = atoi(argv[2]);
	verbosity = atoi(argv[3]);
	loadData();
	doFunction();
	free(array);
	return 0;
}
