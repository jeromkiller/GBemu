//includes
#include "GameBoy.h"
#include "IO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct
struct GameBoy_Instance_t
{
	//shared thread data
	emu_status_flags* emu_status;
	player_input* input;
	framebuffer* fb;

	//pointers
	CPU* CPU_ref;
	RAM* RAM_ref;
	Interrupt_registers* Interrupt_ref;
	Memory_Mapper* MAPPER_ref;
	screenData* graphics_ref;
	DMA_info* DMA_ref;

	//data
	TimerData Timer;
	player_input_data oldInput;
};

//function implementations

GameBoy_Instance* gameBoy_init(shared_Thread_Blocks* sharedBlocks, char* romPath)
{

	//create the gameboy datastructure
	GameBoy_Instance* newGameBoy = (GameBoy_Instance*)malloc(sizeof(GameBoy_Instance));
	if(NULL == newGameBoy)
	{
		return NULL;
	}
	memset(newGameBoy, 0, sizeof(GameBoy_Instance));

	if(NULL != sharedBlocks)
	{
		//claim ownership of the shared data for this instance of the emulator
		newGameBoy->emu_status = get_shared_status_flags(sharedBlocks);
		claim_thread_data(newGameBoy->emu_status);

		newGameBoy->input = get_shared_player_input(sharedBlocks);
		claim_thread_data(newGameBoy->input);

		newGameBoy->fb = get_shared_framebuffer(sharedBlocks);
		claim_thread_data(newGameBoy->fb);
	}

	//create the datastrucutes required to run the gameboy
	newGameBoy->CPU_ref = CPU_init();
	newGameBoy->RAM_ref = RAM_init();
	newGameBoy->Interrupt_ref = interruptRegisters_init();
	newGameBoy->MAPPER_ref = Mapper_init(romPath, newGameBoy->RAM_ref);
	newGameBoy->graphics_ref = screenData_init();
	newGameBoy->DMA_ref = DMA_init();

	if(NULL == newGameBoy->MAPPER_ref)
	{
		gameBoy_dispose(newGameBoy);
		return NULL;
	}
	return newGameBoy;
}

GameBoy_Instance* gameBoy_dispose(GameBoy_Instance* GameBoy)
{
	if(NULL != GameBoy)
	{
		//first dispose the member structs
		GameBoy->CPU_ref = CPU_dispose(GameBoy->CPU_ref);
		GameBoy->MAPPER_ref = Mapper_dispose(GameBoy->MAPPER_ref);
		GameBoy->RAM_ref = RAM_dispose(GameBoy->RAM_ref);
		GameBoy->Interrupt_ref = interruptRegisters_dispose(GameBoy->Interrupt_ref);
		GameBoy->graphics_ref = screenData_dispose(GameBoy->graphics_ref);
		GameBoy->DMA_ref = DMA_dispose(GameBoy->DMA_ref);

		//then free the gameboy struct itself
		free(GameBoy);
	}
	return NULL;
}

emu_status_flags* gameBoy_getEmuStatus(GameBoy_Instance* GB)
{
	return GB->emu_status;
}

player_input* gameBoy_getInput(GameBoy_Instance* GB)
{
	return GB->input;
}

framebuffer* gameBoy_getFramebuffer(GameBoy_Instance* GB)
{
	return GB->fb;
}

CPU* gameboy_getCPU(GameBoy_Instance* GB)
{
	return GB->CPU_ref;
}

RAM* gameboy_getRAM(GameBoy_Instance* GB)
{
	return GB->RAM_ref;
}

Interrupt_registers* gameboy_getInterruptRegs(GameBoy_Instance* GB)
{
	return GB->Interrupt_ref;
}

Memory_Mapper* gameboy_getMemMapper(GameBoy_Instance* GB)
{
	return GB->MAPPER_ref;
}

screenData* gameboy_getScreenData(GameBoy_Instance* GB)
{
	return GB->graphics_ref;
}

TimerData* gameboy_getTimer(GameBoy_Instance* GB)
{
	return &(GB->Timer);
}

player_input_data* gameboy_getOldInputData(GameBoy_Instance* GB)
{
	return &(GB->oldInput);
}

DMA_info* gameboy_getDMAInfo(GameBoy_Instance* GB)
{
	return GB->DMA_ref;
}
