#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int receiveNumber() {
	int number = 0;
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


int getAmountOfChars(int input) {
	char tmpstr[10];
	snprintf(tmpstr, 11, "%d", input);
	char *ptr = tmpstr;
	int count = 0;
	while (*ptr) {
		if(*ptr != '\0') count++;
		ptr++;
	}
	return count;
}


int getOpcode(int instructionPtr) {
	int length = getAmountOfChars(instructionPtr);
	if (length == 1 || length == 2) return instructionPtr;
	char tmpStr[length];
	snprintf(tmpStr, length+1, "%d", instructionPtr);
	char *ptr;
	if(tmpStr[length-2] == '0') ptr = &tmpStr[length-1];
	else ptr = &tmpStr[length-2];
	return(atoi(ptr));
}

int getValForParamMode(int argNr, int instructionPtr) {
	int length = getAmountOfChars(instructionPtr);
	char tmpStr[length];
	snprintf(tmpStr, length+1, "%d", instructionPtr);
	if ((length - 2 - argNr) < 0 ) return 0;
	int paramMode = (int) tmpStr[length-2-argNr] - 48;
	//printf("instrPtr: %d, tmpStr: %s, ptr: %d, argNr = %d, length = %d\n", instructionPtr, tmpStr, paramMode, argNr, length);
	return paramMode;
}

int doFunction(int *array, int verbosity) {
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
				printf("opcode 3 expects numeric input: ");
				array[arg1] = receiveNumber();
				instructionPtr+=2;
				break;
			case 4 :
				if (verbosity) printf("case 4:\tinstruction: %d,%d\n", instruction, arg1);
				printf("opcode 4 output: %d\n", array[arg1]);
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
				instructionPtr+=4;
				//printf("\n");
				break;
			default:
				fprintf(stderr, "invalid opcode: %d\n", opcode);
				return -1;
		}
	}
	return array[0];
}

int main(int argc, char* argv[]) {
	int magicNumber = 19690720;
	int returnVal = 0;
	int *arrPtr;

	int newArray[] = { 
	#include "./05_input.txt"
	};
	arrPtr = newArray;
	doFunction(arrPtr, 0);

	int newNewArray[] = { 
	#include "./05_input.txt"
	};
	arrPtr = newNewArray;
	doFunction(arrPtr, 0);

	return 0;
}

