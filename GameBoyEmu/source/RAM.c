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

	//setup the default values
	*(newRAM + 0xFF05) = 0;
	*(newRAM + 0xFF06) = 0;
	*(newRAM + 0xFF07) = 0;
	*(newRAM + 0xFF10) = 0x80;
	*(newRAM + 0xFF10) = 0xBF;
	*(newRAM + 0xFF12) = 0xF3;
	*(newRAM + 0xFF14) = 0xBF;
	*(newRAM + 0xFF16) = 0x3F;
	*(newRAM + 0xFF17) = 0;
	*(newRAM + 0xFF19) = 0xBF;
	*(newRAM + 0xFF1A) = 0x7F;
	*(newRAM + 0xFF1B) = 0xFF;
	*(newRAM + 0xFF1C) = 0x9F;
	*(newRAM + 0xFF1E) = 0xBF;
	*(newRAM + 0xFF20) = 0xFF;
	*(newRAM + 0xFF21) = 0;
	*(newRAM + 0xFF22) = 0;
	*(newRAM + 0xFF23) = 0xBF;
	*(newRAM + 0xFF24) = 0x77;
	*(newRAM + 0xFF25) = 0xF3;
	*(newRAM + 0xFF26) = 0xF1;
	*(newRAM + 0xFF40) = 0x91;
	*(newRAM + 0xFF42) = 0;
	*(newRAM + 0xFF43) = 0;
	*(newRAM + 0xFF45) = 0;
	*(newRAM + 0xFF47) = 0xFC;
	*(newRAM + 0xFF48) = 0xFF;
	*(newRAM + 0xFF49) = 0xFF;
	*(newRAM + 0xFF4A) = 0;
	*(newRAM + 0xFF4B) = 0;
	*(newRAM + 0xFFFF) = 0;
	

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