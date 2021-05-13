//local includesk
#include "Tools.h"
#include "CPU.h"
#include "RAM.h"

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
void DumpCPU(CPU* CPU_ptr) 
{
	printf("A: ");
	PrintHex8bit(CPU_ptr->A);
	printf("F: ");
	PrintHex8bit(CPU_ptr->F);
	printf("\nB: ");
	PrintHex8bit(CPU_ptr->B);
	printf("C: ");
	PrintHex8bit(CPU_ptr->C);
	printf("\nD: ");
	PrintHex8bit(CPU_ptr->D);
	printf("E: ");
	PrintHex8bit(CPU_ptr->E);
	printf("\nH: ");
	PrintHex8bit(CPU_ptr->H);
	printf("L: ");
	PrintHex8bit(CPU_ptr->L);
	printf("\nSP: ");
	PrintHex16bit(CPU_ptr->SP);
	printf("\nPC: ");
	PrintHex16bit(CPU_ptr->PC);
	printf("\nFlags:\tZNHCxxxx\n\t");
	PrintBinary(CPU_ptr->F);
	printf("\nClyle: %lu", CPU_ptr->CycleNumber);
	printf("\n\n");
}

//test thingy
void test(void *value1, void *value2) 
{
	printf("print %02x, %02x\n", *(char*)value1 & 0xff, *(char*)value2 & 0xff);
}