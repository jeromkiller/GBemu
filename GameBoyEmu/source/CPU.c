#include "stdafx.h"

struct CPU_struct* CPU_ptr = NULL;

//initiate CPU
void CPU_init(void) 
{
	
	//check if the CPU is already inited
	if(CPU_ptr != NULL)
	{
		return;
	}

	//allocate space for the cpu registers
	CPU_ptr = (struct CPU_struct *)malloc(sizeof(struct CPU_struct));

	//set all registers to value 0;
	memset(CPU_ptr, 0, sizeof(struct CPU_struct));

	//set the stack pointer to location 0x0100
	*REG_PC = 0x0100;
}

//free the cpu registers
void CPU_dispose(void) 
{
	//check if the CPU_ptr exists, and free it
	if (NULL != CPU_ptr) 
	{
		free(CPU_ptr);
		CPU_ptr = NULL;
	}
}

//read the value in memory at the Program Counter, increase it by 1 and return the read value.
char Read_PC(void) 
{
	return *(RAM_START + (*REG_PC)++);
}
