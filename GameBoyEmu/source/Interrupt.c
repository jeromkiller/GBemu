#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Interrupt.h"
#include "RAM.h"
#include "Opcodes.h"

//defines

#define INTERRUPT_JUMP_VBLANK (0x0040)
#define INTERRUPT_JUMP_LCDCSTATUS (0x0048)
#define INTERRUPT_JUMP_TIMER (0x0050)
#define INTERRUPT_JUMP_SERIAL (0x0058)
#define INTERRUPT_JUMP_BUTTONS (0x0060)

//local functions
void perform_interrupt(unsigned char InterruptLocation, Interrupt_registers* interrupt_ptr, CPU* CPU_ptr, RAM* RAM_ptr);


Interrupt_registers* interruptRegisters_init()
{
    //allocate space for the new interrupt registers
    Interrupt_registers* newInterupts = (Interrupt_registers*)malloc(sizeof(Interrupt_registers));

    //set all the values to 0
    memset(newInterupts, 0, sizeof(Interrupt_registers));

    //initialize the interrupt values
    newInterupts->CPU_status = CPU_RUNNING;
    newInterupts->Interrupt_master_enable = 0x00;

    return newInterupts;
}

void interruptRegisters_dispose(Interrupt_registers* interrupt_data)
{
    if(NULL != interrupt_data)
    {
        free(interrupt_data);
        interrupt_data = NULL;
    }
}

//main function for handeling interrupts
void check_interrupts(Interrupt_registers* interrupt_ptr, CPU* CPU_ptr, RAM* RAM_ptr)
{
    //check if the IME is set
    switch(interrupt_ptr->Interrupt_master_enable)
    {
        case INTERRUPT_DISABLED:
            break;
        case INTERRUPT_REENABLE_SHEDUELED:
            interrupt_ptr->Interrupt_master_enable = INTERRUPT_REENABLE_NEXT;
            break;
        case INTERRUPT_REENABLE_NEXT:
            interrupt_ptr->Interrupt_master_enable = INTERRUPT_ENABLED;
            break;
        case INTERRUPT_ENABLED:
            break;  //continue with the rest 
        default:
            printf("ERROR: Unknown interrupt state\n");
            return;
    }

    //check if any of the intterupt flags are set
    interruptFlags_Data flags;
    interruptFlags_Data enableFlags;
    flags.value = *(RAM_ptr + RAM_LOCATION_IO_IF);
    enableFlags.value = *(RAM_ptr + RAM_LOCATION_INTERRUPT_ENABLE);

    //and the flags and enable flags to see what interrupts we have to handle
    flags.value &= enableFlags.value;
    
    //set the cpu back to running if it was halted or stopped
    if((flags.value && interrupt_ptr->CPU_status == CPU_HALTED) ||  //if in interrupt happened while the cpu is halted
        (flags.interruptFlags.playerInput && interrupt_ptr->CPU_status == CPU_STOPPED)) //or if a button was pressen while the cpu was stopped
    {
        interrupt_ptr->CPU_status = CPU_RUNNING;
        return;
    }

    if(interrupt_ptr->Interrupt_master_enable != INTERRUPT_ENABLED)
    {
        return;
    }
    

    if(flags.interruptFlags.VBlank)
    {
        flags.interruptFlags.VBlank = 0;
        perform_interrupt(INTERRUPT_JUMP_VBLANK, interrupt_ptr, CPU_ptr, RAM_ptr);
    }
    else if(flags.interruptFlags.LCDC)
    {
        flags.interruptFlags.LCDC = 0;
        perform_interrupt(INTERRUPT_JUMP_LCDCSTATUS, interrupt_ptr, CPU_ptr, RAM_ptr);
    }
    else if(flags.interruptFlags.timerOverflow)
    {
        flags.interruptFlags.timerOverflow = 0;
        perform_interrupt(INTERRUPT_JUMP_TIMER, interrupt_ptr, CPU_ptr, RAM_ptr);
    }
    else if(flags.interruptFlags.serialComplete)
    {
        flags.interruptFlags.serialComplete = 0;
        perform_interrupt(INTERRUPT_JUMP_SERIAL, interrupt_ptr, CPU_ptr, RAM_ptr);
    }
    else if(flags.interruptFlags.playerInput)
    {
        flags.interruptFlags.playerInput = 0;
        perform_interrupt(INTERRUPT_JUMP_SERIAL, interrupt_ptr, CPU_ptr, RAM_ptr);
    }

    

    //set the acnowleged flags
    *(RAM_ptr + RAM_LOCATION_IO_IF) = flags.value;
}

void perform_interrupt(unsigned char InterruptLocation, Interrupt_registers* interrupt_ptr, CPU* CPU_ptr, RAM* RAM_ptr)
{
    //disable the interrupts
    interrupt_ptr->Interrupt_master_enable = INTERRUPT_DISABLED;

    //push the program counter to the stack
    PUSH_Value(CPU_ptr->PC, CPU_ptr, RAM_ptr);

    //jump to the interrupt address
    CPU_ptr->PC = InterruptLocation;
}