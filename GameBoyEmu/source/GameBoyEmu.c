// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "CPU.h"
#include "RAM.h"
#include "Opcodes.h"

#include "Tools.h"
//#include "OpcodeLookupTable.h"

int main()
{
	//startup
	RAM* GameboyRAM = RAM_init();
	CPU* GameboyCPU = CPU_init(GameboyRAM);

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	GameboyCPU->A = 0x40;
	GameboyCPU->B = 0x50;

	printCPU(GameboyCPU);

	OP_SUB(&(GameboyCPU->A), &(GameboyCPU->B), GameboyCPU);

	printCPU(GameboyCPU);

///////////////////////////////////////////////////////////////////////

	CPU_dispose(GameboyCPU);
	RAM_dispose(GameboyRAM);
    return 0;
}