#pragma once

//includes
#include "GB_type.h"
#include "CPU.h"
#include "RAM.h"
#include "Interrupt.h"
#include "RomMapper.h"
#include "sharedData.h"
#include "Graphics.h"

//structures
typedef struct TimerData_t
{
	unsigned long CycleNumber;  //number of cpu cycles
	unsigned char CycleDelta;   //number of cycles spent for each opcode
	unsigned short SystemTimer; //number of clock cycles, rolls over and gets reset by writing to DIV
	unsigned short LastSystemTimer;
	unsigned short TimerStep;   //counter that counts when the next increase
}TimerData;

//functions
//build the gameboy structure, and all associated data structures
GameBoy_Instance* gameBoy_init(shared_Thread_Blocks* sharedBlocks, char* romPath);

GameBoy_Instance* gameBoy_dispose(GameBoy_Instance* GB);

//some getters for easy access
emu_status_flags* gameBoy_getEmuStatus(GameBoy_Instance* GB);
player_input* gameBoy_getInput(GameBoy_Instance* GB);
framebuffer* gameBoy_getFramebuffer(GameBoy_Instance* GB);
CPU* gameboy_getCPU(GameBoy_Instance* GB);
RAM* gameboy_getRAM(GameBoy_Instance* GB);
Interrupt_registers* gameboy_getInterruptRegs(GameBoy_Instance* GB);
Memory_Mapper* gameboy_getMemMapper(GameBoy_Instance* GB);
screenData* gameboy_getScreenData(GameBoy_Instance* GB);
TimerData* gameboy_getTimer(GameBoy_Instance* GB);
player_input_data* gameboy_getOldInputData(GameBoy_Instance* GB);

