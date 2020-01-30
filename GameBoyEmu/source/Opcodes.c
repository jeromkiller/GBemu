#include "stdafx.h"

//this is here for debug
void Func(void *value1, void *value2) {
	printf("test\n\n");
}

struct Instruction_struct get_NormalOpcode(char instruction)
{
	//Can't currently make this lookup table static, because the registers are pointers, wich can't be static because whats the point of having imutable cpu registers :P
	struct Instruction_struct LookupTable[3] = {
		{ _8bitADDliteral, REG_A, REG_F },
		{ _16bitADDliteral, REG_BC, REG_AF },
		{ Func, NULL, NULL }
	};

	return LookupTable[instruction];
}

//functions for normal opcodes

//cpu instructions
void _8bitADDliteral(void *value1, void *value2) 
{
	*(char *)value1 += *(char *)value2;
}

void _16bitADDliteral(void *value1, void *value2)
{
	*(short *)value1 += *(short *)value2;
}


//functions for CB prefixed opcodes