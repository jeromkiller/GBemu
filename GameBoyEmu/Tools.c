#include "stdafx.h"

//prints a single opcode
void PrintOpcode(struct DB_Opcode Opcode) 
{
	printf("%s\t", Opcode.Instruction);
	printf("%s,%s\t", Opcode.Param1, Opcode.Param2);
	PrintHex8bit(Opcode.Opcode);
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
void PrintHex8bit(char value)
{
	//print the hex value in with, with a mask because the value gets promoted to an int
	printf("%02x\t", value & 0xff);
}

void PrintHex16bit(short value)
{
	//print the hex value in with, with a mask because the value gets promoted to an int
	printf("%04x\t", value & 0xffff);
}

//dumps the values in the CPU registers
void DumpCPU() {
	printf("A: ");
	PrintHex8bit(REG_A);
	printf("F: ");
	PrintHex8bit(REG_F);
	printf("\nB: ");
	PrintHex8bit(REG_B);
	printf("C: ");
	PrintHex8bit(REG_C);
	printf("\nD: ");
	PrintHex8bit(REG_D);
	printf("E: ");
	PrintHex8bit(REG_E);
	printf("\nH: ");
	PrintHex8bit(REG_H);
	printf("L: ");
	PrintHex8bit(REG_L);
	printf("\nSP: ");
	PrintHex16bit(REG_SP);
	printf("\nPC: ");
	PrintHex16bit(REG_PC);
	printf("\nFlags:\tZNHCxxxx\n\t");
	PrintBinary(REG_F);
	printf("\n");
}