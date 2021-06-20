// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "CPU.h"
#include "RAM.h"
#include "RomMapper.h"
#include "Interrupt.h"
#include "Opcodes.h"
#include "SerialPrinter.h"

#include "Tools.h"
//#include "OpcodeLookupTable.h"

int main()
{
	static char ROM_Path[] = {"./.roms/cpu_instrs.gb\0"};
	//startup
	GameBoy_Instance* GameBoy = gameBoy_init(ROM_Path);
	if(NULL == GameBoy)
	{
		printf("ERROR: A problem occured whilst creating the gameboy datastructure");
		return 0;
	}

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	while(1)
	{
		performNextOpcode(GameBoy);
		perform_serialOperation(getRAM(GameBoy));
		printf("PC: ");
		printHex16bit(getCPU(GameBoy)->PC);
		printf("\n");
		fflush(stdout);
	}	

///////////////////////////////////////////////////////////////////////
	gameBoy_dispose(GameBoy);

    return 0;
}