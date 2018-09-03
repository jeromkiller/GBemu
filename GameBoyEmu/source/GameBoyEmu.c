// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "stdafx.h"

void Func(int value) {
	printf("%d: test\n\n", value);
}

int main()
{
	CPU_init();
	RAM_init();

	//some user code for testing
	//insert hello world at memory location 0x0100
	char *alpha = "Hello_World\n\0";
	char *loc = RAM_START + 0x0100;
	for (int i = 0; i < 13; i++) {
		*(loc + i) = *(alpha + i);
	}

	//small table of some fake instructions
	struct Instruction_struct LookupTable[3] = {
		{ _8bitADDliteral, &REG_A, &REG_F },{ _16bitADDliteral, &REG_BC, &REG_AF },{ test, &REG_SP, &REG_AF }
	};

	REG_A = 0x2d;
	REG_F = 0x11;
	//REG_D = 0x11;

	//print data in the cpu registers
	DumpCPU();

	//execute the first thing, in this case we add F to A
	LookupTable[0].Instruction(LookupTable[0].value1, LookupTable[0].value2);

	//print the cpu data again, to check if the operation succeded.
	DumpCPU();

	CPU_dispose();
	RAM_dispose();
    return 0;
}