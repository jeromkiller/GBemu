#include "RAM.h"
#include "GameBoy.h"
#include "IO.h"

#include <stdlib.h>
#include <string.h>

//forward declarations
//check if a certain value is in range of rangeStart and rangeEnd
static unsigned char inRange(unsigned short writeLocation, unsigned short RangeStart, unsigned short RangeEnd);
//check if a pointer value is in range of rangeStart and rangeEnd
static unsigned char inPointerRange(void* writeLocation, void* RangeStart, void* RangeEnd);

//function implementations

//initialize RAM
RAM* RAM_init(void) 
{
	/*	the gameboy cpu can adress memory locations from 0x0000 to 0xffff
		different parts of memory are linked to different parts of the hardware
		but to the cpu its all the same.	*/ 
	RAM* newRAM = (RAM*)malloc(0x10000);
	memset(newRAM, 0xff, 0x10000);

	//setup the default values
	*(newRAM + 0xFF05) = 0;
	*(newRAM + 0xFF06) = 0;
	*(newRAM + 0xFF07) = 0;
	*(newRAM + 0xFF10) = 0x80;
	*(newRAM + 0xFF10) = 0xBF;
	*(newRAM + 0xFF12) = 0xF3;
	*(newRAM + 0xFF14) = 0xBF;
	*(newRAM + 0xFF16) = 0x3F;
	*(newRAM + 0xFF17) = 0;
	*(newRAM + 0xFF19) = 0xBF;
	*(newRAM + 0xFF1A) = 0x7F;
	*(newRAM + 0xFF1B) = 0xFF;
	*(newRAM + 0xFF1C) = 0x9F;
	*(newRAM + 0xFF1E) = 0xBF;
	*(newRAM + 0xFF20) = 0xFF;
	*(newRAM + 0xFF21) = 0;
	*(newRAM + 0xFF22) = 0;
	*(newRAM + 0xFF23) = 0xBF;
	*(newRAM + 0xFF24) = 0x77;
	*(newRAM + 0xFF25) = 0xF3;
	*(newRAM + 0xFF26) = 0xF1;
	*(newRAM + 0xFF40) = 0x91;
	*(newRAM + 0xFF42) = 0;
	*(newRAM + 0xFF43) = 0;
	*(newRAM + 0xFF45) = 0;
	*(newRAM + 0xFF47) = 0xFC;
	*(newRAM + 0xFF48) = 0xFF;
	*(newRAM + 0xFF49) = 0xFF;
	*(newRAM + 0xFF4A) = 0;
	*(newRAM + 0xFF4B) = 0;
	*(newRAM + 0xFFFF) = 0;
	

	return newRAM;
}

//free the allocated memory stack
RAM* RAM_dispose(RAM* RAM_ptr) 
{
	//check if the RAM_ptr exists, and free it
	if (NULL != RAM_ptr)
	{
		free(RAM_ptr);
	}
	return NULL;
}

//get 8bit val
unsigned char get_8bitval(void* value, GameBoy_Instance* GB)
{
	return *(unsigned char*)value;
}

//get 16bit val
unsigned short get_16bitval(void* value, GameBoy_Instance* GB)
{
	unsigned char* byte = (unsigned char*)value;
	return (*byte | (*(byte + 1) << 8));
}

//set 16bit val
void set_16bitval(void* writeloc, unsigned short val, GameBoy_Instance* GB)
{
	unsigned char byte1 = val & 0x00FF;
	unsigned char byte2 = (val >> 8) & 0x00FF;
	set_8bitval((unsigned char*)writeloc, byte1, GB);
	set_8bitval((unsigned char*)writeloc + 1, byte2, GB);
}

void set_8bitval(unsigned char* WriteLoc, unsigned char Val, GameBoy_Instance* GB)
{
	//check if the write is done to a special part of memory
	RAM* ram = gameboy_getRAM(GB);
	if(inPointerRange(WriteLoc, ram + RAM_START, ram + RAM_END))
	{
		unsigned short writeLocation = 0;
		if(WriteLoc > ram)
		{
			writeLocation = (unsigned short)(WriteLoc - ram);
		}
		else
		{
			writeLocation = (unsigned short)(ram - WriteLoc);
		}
		
		//check if the write is to a special address
		switch(writeLocation)
		{
			case RAM_LOCATION_IO_JOYPAD:
			{
				write_to_input(gameboy_getRAM(GB), gameboy_getOldInputData(GB), Val);
				return;
			}
			case RAM_LOCATION_IO_DIV:
			{
				//reset the systemTimer, this resets div and affects the timer
				write_to_DIV(ram, gameboy_getTimer(GB));
				return;
			}
			case RAM_LOCATION_IO_TAC:
			{
				//If the clock select changes, the TimerStep has to be recalculated
				write_to_TAC(Val, gameboy_getTimer(GB));
				break;
			}
			case RAM_LOCATION_IO_TIMA:
			{
				//nothing special, just for debugging
				TimerData* timer = gameboy_getTimer(GB);
				timer->TimerStep = timer->TimerStep;
				break;
			}
			default:
			//check if the write is done to a special address range
			//see if the write is being made to ROM
			if(inRange(writeLocation, RAM_LOCATION_ROM_0_START, RAM_LOCATION_ROM_SWAPPABLE_END))
			{
				write_to_rom(writeLocation, Val, gameboy_getMemMapper(GB), gameboy_getRAM(GB));
				return;
			}
			//check if the write is to cartridge ram while its dissabled
			else if(inRange(writeLocation, RAM_LOCATION_RAM_SWAPPABLE_START, RAM_LOCATION_RAM_SWAPPABLE_END) &&
				!(gameboy_getMemMapper(GB)->ram_enabled))
			{
				return;
			}
		}
	}

	//if none of the above or if a break is used,
	//write the value
	*WriteLoc = Val;
}

static unsigned char inRange(unsigned short writeLocation, unsigned short RangeStart, unsigned short RangeEnd)
{
	return ((writeLocation >= RangeStart) && (writeLocation <= RangeEnd));
}

static unsigned char inPointerRange(void* writeLocation, void* RangeStart, void* RangeEnd)
{
	return ((writeLocation >= RangeStart) && (writeLocation <= RangeEnd));
}