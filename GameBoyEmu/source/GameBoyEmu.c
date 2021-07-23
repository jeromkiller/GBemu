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
//#include "OpcodeLookupTable.h"

int run(shared_Thread_Blocks* threadData);
int checkpipe(int fd);

int startGameboy(shared_Thread_Blocks* threadData_ptr)
{
	return run(threadData_ptr);
}

int run(shared_Thread_Blocks* threadData)
{
	//get the in and output pipes
	int readFD = threadData->gui_pipe[PIPE_READ];
	int writeFD = threadData->emu_pipe[PIPE_WRITE];
//	close(threadData->gui_pipe[PIPE_WRITE]);
//	close(threadData->emu_pipe[PIPE_READ]);
	

	static char ROM_Path[] = {"./.roms/testRoms/cpu_instrs/cpu_instrs.gb\0"};
	//startup
	GameBoy_Instance* GameBoy = gameBoy_init(threadData, ROM_Path);
	if(NULL == GameBoy)
	{
		printf("ERROR: A problem occured whilst creating the gameboy datastructure");
		return 0;
	}

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	while(getInterruptRegs(GameBoy)->CPU_status != CPU_STOPPED)
	{
		//check if we have to quit
		if(checkpipe(readFD))
		{
			break;
		}

		check_interrupts(getInterruptRegs(GameBoy), getCPU(GameBoy), getRAM(GameBoy));
		performNextOpcode(GameBoy);
		perform_serialOperation(getRAM(GameBoy));
		perform_timerOperation(getRAM(GameBoy), GameBoy->SystemTimer, &(GameBoy->LastSystemTimer), &(GameBoy->TimerStep));
		fflush(stdout);
	}

///////////////////////////////////////////////////////////////////////
//cleanup
	gameBoy_dispose(GameBoy);

    return 0;
}

int checkpipe(int fd)
{
	//pipe read should be nonblocking
	char inchars[2] = " "; //should be large enough, only one byte will be sent
	int readchars = read(fd, inchars, 1);
	if(readchars < 0)
	{
		int error = errno;
	}
	if(readchars > 0)
	{
		return 1;
	}
	return 0;
}