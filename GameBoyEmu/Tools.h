#pragma once
#ifndef Tools
#define Tools
//includes
#include <stdio.h>

//defines

//structs
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
void PrintOpcode(struct DB_Opcode Opcode);

//prints an array of opcodes
void PrintAllOpcodes(struct DB_Opcode *Opcode, int count);

//prints an 8 bit value in binary
void PrintBinary(char value);

//prints an 8 bit value in hex
void PrintHex(char value);

//dumps the values in the CPU registers
void DumpCPU();

#endif // !Tools


