//includes
#include "GameBoy.h"
#include "IO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GameBoy_Instance* gameBoy_init(char* romPath)
{
	//create the gameboy datastructure
	GameBoy_Instance* newGameBoy = (GameBoy_Instance*)malloc(sizeof(GameBoy_Instance));
	if(NULL == newGameBoy)
	{
		return NULL;
	}
	memset(newGameBoy, 0, sizeof(GameBoy_Instance));

	//create the datastrucutes required to run the gameboy
	newGameBoy->CPU_ref = CPU_init();
	newGameBoy->RAM_ref = RAM_init();
	newGameBoy->Interrupt_ref = interruptRegisters_init();
	newGameBoy->MAPPER_ref = Mapper_init(romPath, newGameBoy->RAM_ref);
	if(NULL == newGameBoy->MAPPER_ref)
	{
		gameBoy_dispose(newGameBoy);
		return NULL;
	}
	return newGameBoy;
}

void gameBoy_dispose(GameBoy_Instance* GameBoy)
{
	if(NULL != GameBoy)
	{
		//first dispose the member structs
		CPU_dispose(GameBoy->CPU_ref);
		Mapper_dispose(GameBoy->MAPPER_ref);
		RAM_dispose(GameBoy->RAM_ref);
		interruptRegisters_dispose(GameBoy->Interrupt_ref);

		//then free the gameboy struct itself
		free(GameBoy);
		GameBoy = NULL;
	}
}

CPU* getCPU(GameBoy_Instance* GB)
{
	return GB->CPU_ref;
}

RAM* getRAM(GameBoy_Instance* GB)
{
	return GB->RAM_ref;
}

Interrupt_registers* getInterruptRegs(GameBoy_Instance* GB)
{
	return GB->Interrupt_ref;
}

Memory_Mapper* getMemMapper(GameBoy_Instance* GB)
{
	return GB->MAPPER_ref;
}

unsigned char inRange(unsigned short writeLocation, unsigned short RangeStart, unsigned short RangeEnd)
{
	return ((writeLocation >= RangeStart) && (writeLocation <= RangeEnd));
}


void writeOperation(unsigned char* value1, unsigned char* value2, GameBoy_Instance* GB)
{
	//check if the write is done to a special part of memory
	RAM* ram = getRAM(GB);
	unsigned short writeLocation = value1 - ram;
	if(inRange(writeLocation, RAM_START, RAM_END))
	{
		//check if the write is to a special address
		switch(writeLocation)
		{
			case RAM_LOCATION_IO_DIV:
				//reset the systemTimer, this resets div and affects the timer
				GB->SystemTimer = 0;
				GB->LastSystemTimer = 0;
				GB->TimerStep = 0;
				ram[RAM_LOCATION_IO_DIV] = 0;
				return;
			case RAM_LOCATION_IO_TAC:
				//If the clock select changes, the TimerStep has to be recalculated
				GB->TimerStep = GB->TimerStep % IO_TIMER_CLOCKSELECT[*value2 & TIMER_TAC_CLOCKSELECT_BITS];
				break;
			case RAM_LOCATION_IO_TIMA:
				//nothing special, just for debugging
				GB->TimerStep = GB->TimerStep;
				break;
			default:
			//check if the write is done to a special address range
			//see if the write is being made to ROM
			if(inRange(writeLocation, RAM_LOCATION_ROM_0_START, RAM_LOCATION_ROM_SWAPPABLE_END))
			{
				write_to_rom(writeLocation, *value2, getMemMapper(GB), getRAM(GB));
				return;
			}
			//check if the write is to cartridge ram while its dissabled
			else if(inRange(writeLocation, RAM_LOCATION_RAM_SWAPPABLE_START, RAM_LOCATION_RAM_SWAPPABLE_END) &&
				!(getMemMapper(GB)->ram_enabled))
			{
				return;
			}
		}
	}

	//if none of the above or if a break is used,
	//write the value
	*value1 = *value2;
}