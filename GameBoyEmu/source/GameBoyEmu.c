// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "CPU.h"
#include "RAM.h"
#include "RomMapper.h"
#include "Interrupt.h"
#include "Opcodes.h"
#include "IO.h"

#include "Tools.h"
//#include "OpcodeLookupTable.h"

int main()
{
	static char ROM_Path[] = {"./.roms/testRoms/09-op r,r.gb\0"};
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
		check_interrupts(getInterruptRegs(GameBoy), getCPU(GameBoy), getRAM(GameBoy));
		performNextOpcode(GameBoy);
		perform_serialOperation(getRAM(GameBoy));
		perform_timerOperation(getRAM(GameBoy), GameBoy->SystemTimer, &(GameBoy->LastSystemTimer), &(GameBoy->TimerStep));
		fflush(stdout);
	}	


///////////////////////////////////////////////////////////////////////
	gameBoy_dispose(GameBoy);

    return 0;
}
