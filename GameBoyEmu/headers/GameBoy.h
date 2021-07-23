#pragma once

//includes
#include "CPU.h"
#include "RAM.h"
#include "Interrupt.h"
#include "RomMapper.h"
#include "sharedData.h"

//struct
typedef struct GameBoy_Struct
{
	//shared thread data
	player_input* input;
	framebuffer* fb;

	//pointers
	CPU* CPU_ref;
	RAM* RAM_ref;
	Interrupt_registers* Interrupt_ref;
	Memory_Mapper* MAPPER_ref;

	//data
	unsigned long CycleNumber;  //number of cpu cycles
	unsigned short SystemTimer; //number of clock cycles, rolls over and gets reset by writing to DIV
	unsigned short LastSystemTimer;
	unsigned short TimerStep;   //counter that counts when the next increase
}GameBoy_Instance;

//functions
//build the gameboy structure, and all associated data structures
GameBoy_Instance* gameBoy_init(shared_Thread_Blocks* sharedBlocks, char* romPath);

void gameBoy_dispose(GameBoy_Instance* GB);

//some getters for easy access
CPU* getCPU(GameBoy_Instance* GB);
RAM* getRAM(GameBoy_Instance* GB);
Interrupt_registers* getInterruptRegs(GameBoy_Instance* GB);
Memory_Mapper* getMemMapper(GameBoy_Instance* GB);


void writeOperation(unsigned char* value1, unsigned char* value2, GameBoy_Instance* GB);
