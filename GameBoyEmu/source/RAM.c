#include "RAM.h"

#include <stdlib.h>
#include <string.h>

//initialize RAM
RAM* RAM_init(void) 
{
	/*	the gameboy cpu can adress memory locations from 0x0000 to 0xffff
		different parts of memory are linked to different parts of the hardware
		but to the cpu its all the same.	*/ 
	RAM* newRAM = (RAM*)malloc(0xffff);
	memset(newRAM, 0, 0xffff);

	return newRAM;
}

//free the allocated memory stack
void RAM_dispose(RAM* RAM_ptr) 
{
	//check if the RAM_ptr exists, and free it
	if (NULL != RAM_ptr)
	{
		free(RAM_ptr);
		RAM_ptr = NULL;
	}
}