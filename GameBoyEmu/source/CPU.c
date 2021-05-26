#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CPU.h"
#include "RAM.h"

//initiate CPU
CPU* CPU_init(RAM* RAM_ptr, Memory_Mapper* Mapper_ptr)
{
	//check if the CPU is already inited
	if(RAM_ptr == NULL)
	{
		printf("CPU_init: Please Initialize Ram before the CPU\n");
		return NULL;
	}

	//allocate space for the cpu registers
	CPU* newCPU = (CPU*)malloc(sizeof(CPU));

	//set all registers to value 0;
	memset(newCPU, 0, sizeof(CPU));

	//copy the ram refference
	newCPU->RAM_ref = RAM_ptr;
	newCPU->MAPPER_ref = Mapper_ptr;
	//set the Program Counter to location 0x0100
	newCPU->PC = 0x0100;
	newCPU->SP = 0xFFFE;

	return newCPU;
}

//free the cpu registers
void CPU_dispose(CPU* CPU_ptr) 
{
	//check if the CPU_ptr exists, and free it
	if (NULL != CPU_ptr) 
	{
		free(CPU_ptr);
		CPU_ptr = NULL;
	}
}

//read the value in memory at the Program Counter, increase it by 1 and return the read value.
unsigned char* Read_PC8(CPU* CPU_ptr) 
{
	return (CPU_ptr->RAM_ref + (CPU_ptr->PC)++);
}

//read the next two bytes, increase the pc by 2
unsigned short* Read_PC16(CPU* CPU_ptr)
{
	unsigned short* ramLocation = (unsigned short*)(CPU_ptr->RAM_ref + CPU_ptr->PC);
	CPU_ptr->PC += 2;
	return ramLocation;
}
