#include "stdafx.h"

//initiate CPU
void CPU_init(void) {
	//allocate space for the cpu registers
	CPU_ptr = (struct CPU_struct *)malloc(sizeof(struct CPU_struct));

	//set all registers to value 0;
	char* ptr = (char *)CPU_ptr;
	for (int i = 0; i < sizeof(struct CPU_struct); i++) {
		*(ptr + i) = 0;
	}

	//set the stack pointer to location 0x0100
	REG_PC = 0x0100;
}

//free the cpu registers
void CPU_dispose(void) {
	//check if the CPU_ptr exists, and free it
	if (NULL != CPU_ptr) {
		free(CPU_ptr);
		CPU_ptr = NULL;
	}
}

//read the value in memory at the Program Counter, increase it by 1 and return the read value.
char Read_PC(void) {
	return *(RAM_START + REG_PC++);
}

//cpu instructions
void _8bitADDliteral(void *value1, void *value2) {
	*(char *)value1 += *(char *)value2;
}

void _16bitADDliteral(void *value1, void *value2){
	*(short *)value1 += *(short *)value2;
}