#pragma once
#ifndef Tools_h
#define Tools_h
//includes
#include <stdio.h>

//defines

//enums

//global variables

//function prototypes
//prints a single opcode

//prints an 8 bit value in binary
void PrintBinary(char value);

//prints an 8 bit value in hex
void PrintHex8bit(char value);

//prints an 16 bit value in hex
void PrintHex16bit(short value);

//dumps the values in the CPU registers
void DumpCPU();

//test
void test(void *value1, void *value2);

#endif // !Tools
