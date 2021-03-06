#include "stdafx.h"

//prints an 8 bit value in binary
void PrintBinary(char value)
{
	unsigned char bit = 0x80;
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
void DumpCPU() 
{
	printf("A: ");
	PrintHex8bit(*REG_A);
	printf("F: ");
	PrintHex8bit(*REG_F);
	printf("\nB: ");
	PrintHex8bit(*REG_B);
	printf("C: ");
	PrintHex8bit(*REG_C);
	printf("\nD: ");
	PrintHex8bit(*REG_D);
	printf("E: ");
	PrintHex8bit(*REG_E);
	printf("\nH: ");
	PrintHex8bit(*REG_H);
	printf("L: ");
	PrintHex8bit(*REG_L);
	printf("\nSP: ");
	PrintHex16bit(*REG_SP);
	printf("\nPC: ");
	PrintHex16bit(*REG_PC);
	printf("\nFlags:\tZNHCxxxx\n\t");
	PrintBinary(*REG_F);
	printf("\n\n");
}

//test thingy
void test(void *value1, void *value2) 
{
	printf("print %02x, %02x\n", *(char*)value1 & 0xff, *(char*)value2 & 0xff);
}