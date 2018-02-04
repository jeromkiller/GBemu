#include "stdafx.h"

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

//dumps the values in the CPU registers
void DumpCPU() {
	printf("A: ");
	PrintHex(REG_A);
	printf("F: ");
	PrintHex(REG_F);
	printf("\nB: ");
	PrintHex(REG_B);
	printf("C: ");
	PrintHex(REG_C);
	printf("\nD: ");
	PrintHex(REG_D);
	printf("E: ");
	PrintHex(REG_E);
	printf("\nH: ");
	PrintHex(REG_H);
	printf("L: ");
	PrintHex(REG_L);
}