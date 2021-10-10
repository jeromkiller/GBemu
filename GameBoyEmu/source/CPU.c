#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CPU.h"
#include "RAM.h"

//initiate CPU
CPU* CPU_init(void)//, Interrupt_registers* Interrupt_ptr)
{
	//allocate space for the cpu registers
	CPU* newCPU = (CPU*)malloc(sizeof(CPU));

	//set all registers to value 0;
	memset(newCPU, 0, sizeof(CPU));

	//set the starput values
	newCPU->AF = 0x01B0;
	newCPU->BC = 0x0013;
	newCPU->DE = 0x00D8;
	newCPU->HL = 0x014D;
	
	newCPU->PC = 0x0100;
	newCPU->SP = 0xFFFE;

	return newCPU;
}

//free the cpu registers
CPU* CPU_dispose(CPU* CPU_ptr) 
{
	//check if the CPU_ptr exists, and free it
	if (NULL != CPU_ptr) 
	{
		free(CPU_ptr);
	}
	return NULL;
}

CPU_flags* getFlags(CPU* CPU_ptr)
{
	return &(CPU_ptr->FLAGS);
}