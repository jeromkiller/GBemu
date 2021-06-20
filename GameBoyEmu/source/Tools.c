//local includesk
#include "Tools.h"
#include "GameBoy.h"

//prints an 8 bit value in binary
void printBinary(char value)
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
void printHex8bit(char value)
{
	//print the hex value in with, with a mask because the value gets promoted to an int
	printf("%02x\t", value & 0xff);
}

void printHex16bit(short value)
{
	//print the hex value in with, with a mask because the value gets promoted to an int
	printf("%04x\t", value & 0xffff);
}

//dumps the values in the CPU registers
void printCPU(CPU* CPU_ptr) 
{
	printf("A: ");
	printHex8bit(CPU_ptr->A);
	printf("F: ");
	printHex8bit(CPU_ptr->F);
	printf("\nB: ");
	printHex8bit(CPU_ptr->B);
	printf("C: ");
	printHex8bit(CPU_ptr->C);
	printf("\nD: ");
	printHex8bit(CPU_ptr->D);
	printf("E: ");
	printHex8bit(CPU_ptr->E);
	printf("\nH: ");
	printHex8bit(CPU_ptr->H);
	printf("L: ");
	printHex8bit(CPU_ptr->L);
	printf("\nSP: ");
	printHex16bit(CPU_ptr->SP);
	printf("\nPC: ");
	printHex16bit(CPU_ptr->PC);
	printf("\nFlags:\tZNHCxxxx\n\t");
	printBinary(CPU_ptr->F);
	printf("\n\n");
}

//test thingy
void test(void *value1, void *value2) 
{
	printf("print %02x, %02x\n", *(char*)value1 & 0xff, *(char*)value2 & 0xff);
}

//prints the last 10 entries of 
void printStack(GameBoy_Instance* GB)
{
	CPU* CPU_ptr = getCPU(GB);
	RAM* RAM_ptr = getRAM(GB);

	int location = CPU_ptr->SP + 20;
	if(location >= 0xFFFE)
	{
		location = 0xFFFE;
		printf("== stack top ==\n");
	}

	for(; location >= CPU_ptr->SP; location -= 2)
	{
		printHex16bit(location);
		printf("-> 0x");
		printHex16bit(*(unsigned short*)(RAM_ptr + location));
		printf(";\n");
	}
	printf("== ========= ==\n\n");
}