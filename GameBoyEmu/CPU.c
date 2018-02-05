#include "stdafx.h"

//initiate CPU
int CPU_init(void) {
	//allocate space for the cpu registers
	(struct CPU_struct *)CPU_ptr = (struct CPU_struct *)malloc(sizeof(struct CPU_struct));

	//set all registers to value 0;
	char* ptr = (char *)CPU_ptr;
	for (int i = 0; i < sizeof(struct CPU_struct); i++) {
		*(ptr + i) = 0;
	}

	//int i = 5;
	return 0;
}
