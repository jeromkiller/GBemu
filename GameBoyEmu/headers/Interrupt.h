#pragma once

#include "RAM.h"
#include "CPU.h"

//Enums
typedef enum CPU_statuses
{
	CPU_STOPPED = 0,
	CPU_HALTED,
	CPU_RUNNING,
}CPU_status;

typedef enum Interrupt_statusses
{
	INTERRUPT_DISABLED = 0,
	INTERRUPT_ENABLED,
	INTERRUPT_REENABLE_NEXT,
	INTERRUPT_REENABLE_SHEDUELED, //when reenabeling the innterrupts, they only enable after the next opcode
}Interrupt_status;

//structs
typedef struct Interrupt_registers_struct
{
	CPU_status CPU_status;
	Interrupt_status Interrupt_master_enable;
}Interrupt_registers;

typedef union interruptFlags
{
	unsigned char value;
	struct
	{
		unsigned char VBlank: 1;
		unsigned char LCDC: 1;
		unsigned char timerOverflow: 1;
		unsigned char serialComplete: 1;
		unsigned char playerInput: 1;
		unsigned char null: 3;
	};
}interruptFlags;

//Functions
//build the interrupt registers
Interrupt_registers* interruptRegisters_init();

//free the struct for the interrupt registers
void interruptRegisters_dispose(Interrupt_registers* interrupt_data);

//main function for handeling interrupts
void check_interrupts(Interrupt_registers* interrupt_ptr, CPU* CPU_ptr, RAM* RAM_ptr);
