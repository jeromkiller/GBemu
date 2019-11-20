
#include "stdafx.h"

//initialize RAM
void RAM_init(void) {
	/*	the gameboy cpu can adress memory locations from 0x0000 to 0xffff
		different parts of memory are linked to different parts of the hardware
		but to the cpu its all the same.	*/ 
	RAM_ptr = (char *)malloc(0xffff);
}

//free the allocated memory stack
void RAM_dispose(void) {
	//check if the RAM_ptr exists, and free it
	if (NULL != RAM_ptr) {
		free(RAM_ptr);
		RAM_ptr = NULL;
	}
}
