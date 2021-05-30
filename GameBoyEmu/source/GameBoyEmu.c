// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "CPU.h"
#include "RAM.h"
#include "RomMapper.h"
#include "Opcodes.h"
#include "SerialPrinter.h"

#include "Tools.h"
//#include "OpcodeLookupTable.h"

int main()
{
	static char ROM_Path[] = {"./.roms/testRoms/cpu_instrs.gb\0"};
	//startup
	RAM* GameboyRAM = RAM_init();
	Memory_Mapper* mapper = Mapper_init(ROM_Path, GameboyRAM);
	CPU* GameboyCPU = CPU_init(GameboyRAM, mapper);

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	while(GameboyCPU->status == RUNNING)
	{
		performNextOpcode(GameboyCPU);
		perform_serialOperation(GameboyRAM);
//		printf("PC: ");
//		printHex16bit(GameboyCPU->PC);
//		printf("\n");
		fflush(stdout);
	}	

///////////////////////////////////////////////////////////////////////

	CPU_dispose(GameboyCPU);
	Mapper_dispose(mapper);
	RAM_dispose(GameboyRAM);
    return 0;
}