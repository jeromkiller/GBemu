#include "stdafx.h"
#include "Tools.h"

//prints a single opcode
void PrintOpcode(struct DB_Opcode Opcode) 
{
	printf("%s\t", Opcode.Instruction);
	printf("%s,%s\t", Opcode.Param1, Opcode.Param2);
	PrintHex(Opcode.Opcode);
	PrintBinary(Opcode.Opcode);
	printf("%d\n", Opcode.Cycles);
}

//prints an array of opcodes
void PrintAllOpcodes(struct DB_Opcode *Opcode, int count) 
{
	printf("==============================================\n");

	for (int i = 0; i < count; i++) {
		PrintOpcode(*(Opcode + i));
	}

	printf("==============================================\n");
}

//prints an 8 bit value in binary
void PrintBinary(char value)
{
	unsigned char bit = 128;
	for (char i = 0; i < 8; i++) {
		if ((bit >> i) & value) {
			printf("1");
		}
		else {
			printf("0");
		}
	}
	printf("\t");
}

//prints an 8 bit value in hex
void PrintHex(char value)
{
	printf("%02x\t", value);
}