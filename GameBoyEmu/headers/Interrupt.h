#pragma once

//includes
//#include "Gameboy.h"

//Enums
typedef enum CPU_statuses
{
	STOPPED = 0,
	HALTED,
	RUNNING,
}CPU_status;

typedef enum Interrupt_statusses
{
    DISABLED = 0,
    ENABLED,
    REENABLE_NEXT,
    REENABLE_SHEDUELED, //when reenabeling the innterrupts, they only enable after the next opcode
}Interrupt_status;

//structs
typedef struct Interrupt_registers_struct
{
    CPU_status CPU_status;
    Interrupt_status Interrupt_master_enable;
    unsigned char Interrupt_flag;
}Interrupt_registers;

//Functions
//build the interrupt registers
Interrupt_registers* interruptRegisters_init();

//free the struct for the interrupt registers
void interruptRegisters_dispose(Interrupt_registers* interrupt_data);

//main function for handeling interrupts
//void check_interrupts(GameBoy_Instance* GB);
