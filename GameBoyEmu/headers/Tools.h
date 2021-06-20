#pragma once
//Include Tools.h if you want to use debug functions in your file

//includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GameBoy.h"

//prints an 8 bit value in binary
void printBinary(char value);

//prints an 8 bit value in hex
void printHex8bit(char value);

//prints an 16 bit value in hex
void printHex16bit(short value);

//dumps the values in the CPU registers
void printCPU(CPU* CPU_ptr);

//dumps the last 10 entries on the stack
void printStack(GameBoy_Instance* GB);
