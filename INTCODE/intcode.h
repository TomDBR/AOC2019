#ifndef INTCODE_H_
#define INTCODE_H_
int receiveNumber();
void loadData();
int getAmountOfChars(int input);
int getOpcode(int instructionPtr);
int getParameterMode(int argNr, int instructionPtr);
long getArgument(int instructionPtr, int parameterMode, int relativeBase);
long doFunction();
long handleInput(char *file, int verbose);
#endif
