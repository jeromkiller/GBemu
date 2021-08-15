// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "CPU.h"
#include "RAM.h"
#include "RomMapper.h"
#include "Opcodes.h"
#include "IO.h"

//#include "OpcodeLookupTable.h"

//for checking if the cpu is spooling
unsigned short lastAddress = 0;

//returns 1 if the new address of the program counter is the same as it was when it was called last time
//like when a JR -1 happens, so the cpu just spools
unsigned char isSpooling(CPU* CPU_ptr)
{
	unsigned char spooling = lastAddress == CPU_ptr->PC;
	lastAddress = CPU_ptr->PC;

	return spooling;
}

int main(int argc, char *argv[] )
{
	//check the arguments
	if(argc != 2)
	{
		printf("Wierd ammount of parameters found, please use as: %s [rom path]\n", argv[0]);
		return 0;
	}

	char* ROM_Path = argv[1];
	//startup
	GameBoy_Instance* GameBoy = gameBoy_init(NULL, ROM_Path);

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	TimerData* Timer = gameboy_getTimer(GameBoy);
	while(Timer->CycleNumber < 50000000)	//the cpu spools in the interrupt test, so i can't use that as a check anymore :P
	{
		check_interrupts(gameboy_getInterruptRegs(GameBoy), gameboy_getCPU(GameBoy), gameboy_getRAM(GameBoy));
		performNextOpcode(GameBoy);
		perform_serialOperation(gameboy_getRAM(GameBoy));
		perform_timerOperation(gameboy_getRAM(GameBoy), Timer->SystemTimer, &(Timer->LastSystemTimer), &(Timer->TimerStep));
		fflush(stdout);
	}	

///////////////////////////////////////////////////////////////////////

	gameBoy_dispose(GameBoy);
	return 0;
}