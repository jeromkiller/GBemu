// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	//startup
	CPU_init();
	RAM_init();
	//struct Instruction_struct* Normal_Opcodes = Normal_Opcodes_init();

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	//insert hello world at memory location 0x0100
	char *alpha = "Hello_World\n\0";
	char *loc = RAM_START + 0x0100;
	for (int i = 0; i < 13; i++) {		//this could be memcpy'ed, but its just a debug thing
		*(loc + i) = *(alpha + i);
	}

	*REG_A = 0x22;
	*REG_F = 0x10;
	//*REG_D = 0x11;

	//print data in the cpu registers
	DumpCPU();

	//execute the first thing, in this case we add F to A
//	Normal_Opcodes[0].Instruction(Normal_Opcodes[0].value1, Normal_Opcodes[0].value2);
//	Normal_Opcodes[2].Instruction(Normal_Opcodes[2].value1, Normal_Opcodes[2].value2);
	struct Instruction_struct op = get_NormalOpcode(2);
	op.Instruction(op.value1, op.value2);

	FLAG_Z = 1;
	FLAG_H = 1;

	//print the cpu data again, to check if the operation succeded.
	DumpCPU();

///////////////////////////////////////////////////////////////////////

	CPU_dispose();
	RAM_dispose();
    return 0;
}