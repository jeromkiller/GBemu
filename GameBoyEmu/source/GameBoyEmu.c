// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "GameBoyEmu.h"	
#include "CPU.h"
#include "RAM.h"
#include "RomMapper.h"
#include "Interrupt.h"
#include "Opcodes.h"
#include "IO.h"

#include "Tools.h"
//#include "OpcodeLookupTable.h"

int run(thread_data** threadData);

int startGameboy(void* threadData_ptr)
{
	thread_data** sharedData = (thread_data**)(threadData_ptr);
	return run(sharedData);
}

int run(thread_data** threadData)
{
	static char ROM_Path[] = {"./.roms/testRoms/cpu_instrs/cpu_instrs.gb\0"};
	//startup
	GameBoy_Instance* GameBoy = gameBoy_init(ROM_Path);
	if(NULL == GameBoy)
	{
		printf("ERROR: A problem occured whilst creating the gameboy datastructure");
		return 0;
	}

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	while(getInterruptRegs(GameBoy)->CPU_status != CPU_STOPPED)
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
