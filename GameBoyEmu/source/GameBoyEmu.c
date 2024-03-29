// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "GameBoyEmu.h"	
#include "CPU.h"
#include "RAM.h"
#include "RomMapper.h"
#include "Interrupt.h"
#include "Opcodes.h"
#include "IO.h"

#include <unistd.h>
#include <errno.h>

#include "Tools.h"

static int run(shared_Thread_Blocks* threadData);
static int check_emu_status(emu_status_flags* status);

int startGameboy(shared_Thread_Blocks* threadData_ptr)
{
	return run(threadData_ptr);
}

static int run(shared_Thread_Blocks* threadData)
{
	char* ROM_Path = get_romfile(threadData);
	if(NULL == ROM_Path)
	{
		ROM_Path = ".roms/Mario.gb";
	}

	//startup
	GameBoy_Instance* GameBoy = gameBoy_init(threadData, ROM_Path);
	if(NULL == GameBoy)
	{
		printf("ERROR: A problem occured whilst creating the gameboy datastructure");
		return 0;
	}

///////////////////////////////////////////////////////////////////////
	//some user code for testing

	while(!check_emu_status(gameBoy_getEmuStatus(GameBoy)))
	{
		handle_newinput(gameboy_getRAM(GameBoy), gameboy_getOldInputData(GameBoy), gameBoy_getInput(GameBoy));
		check_interrupts(gameboy_getInterruptRegs(GameBoy), GameBoy);
		performNextOpcode(GameBoy);
		perform_serialOperation(gameboy_getRAM(GameBoy));
		TimerData* Timer = gameboy_getTimer(GameBoy);
		perform_timerOperation(gameboy_getRAM(GameBoy), Timer->SystemTimer, &(Timer->LastSystemTimer), &(Timer->TimerStep));
		perform_LCD_operation(gameboy_getScreenData(GameBoy), gameboy_getRAM(GameBoy), gameBoy_getFramebuffer(GameBoy), Timer->CycleDelta, get_DMArunning(gameboy_getDMAInfo(GameBoy)));
		perform_DMAtick(gameboy_getDMAInfo(GameBoy), Timer->CycleDelta);

		fflush(stdout);
	}

///////////////////////////////////////////////////////////////////////
//cleanup
	//release our ownership of the shared data
	destroy_shared_Thread_Blocks(threadData);
	gameBoy_dispose(GameBoy);

    return 0;
}

static int check_emu_status(emu_status_flags* status)
{
	//lock the malloc and get the data
	emu_status_flags_data* status_flags = get_status_flags_data(status);
	status_flags_t flags_value = status_flags->flags;
	unlock_shared_data(status);
	//check if the gameboy is running
	return (flags_value & STATUS_FLAG_EMULATOR_STOPING);
}