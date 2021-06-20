#include <stdlib.h>
#include <string.h>

#include "Interrupt.h"
#include "RAM.h"

#define INTERRUPT_JUMP_VBLANK (0x0040)
#define INTERRUPT_JUMP_LCDCSTATUS (0x0048)
#define INTERRUPT_JUMP_TIMER (0x0050)
#define INTERRUPT_JUMP_SERIAL (0x0058)
#define INTERRUPT_JUMP_BUTTONS (0x0060)

Interrupt_registers* interruptRegisters_init()
{
    //allocate space for the new interrupt registers
    Interrupt_registers* newInterupts = (Interrupt_registers*)malloc(sizeof(Interrupt_registers));

    //set all the values to 0
    memset(newInterupts, 0, sizeof(Interrupt_registers));

    //initialize the interrupt values
    newInterupts->CPU_status = RUNNING;
    newInterupts->Interrupt_flag = 0xe0;
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

//void check_interrupts(GameBoy_Instance* GB)
//{
//    
//}