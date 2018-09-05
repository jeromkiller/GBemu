#pragma once
#ifndef Tools_h
#define Tools_h
//includes
#include <stdio.h>

//defines

//structs
//DEPRECATED: this struct is no longer in use, and should be removed at a later date
struct DB_Opcode {
	char Instruction[4];
	char Param1[4];
	char Param2[4];
	unsigned char Opcode;
	unsigned char Cycles;
};

//enums

//global variables

//function prototypes
//prints a single opcode
//DEPRECATED: this function is no longer in use, and should be removed at a later date
void PrintOpcode(struct DB_Opcode Opcode);

//prints an array of opcodes
//DEPRECATED: this function is no longer in use, and should be removed at a later date
void PrintAllOpcodes(struct DB_Opcode *Opcode, int count);

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
