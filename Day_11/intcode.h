#ifndef INTCODE_H_
#define INTCODE_H_
int receiveNumber();
void loadData();
int getAmountOfChars(int input);
int getOpcode(int instructionPtr);
int getParameterMode(int argNr, int instructionPtr);
int getArgument(int instructionPtr, int parameterMode, int relativeBase);
void doFunction();
int main();
#endif
