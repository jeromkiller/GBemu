#pragma once                                     

//includes
#include "RAM.h"

//defines
//Macro's for ram locations
#define SERIAL_TRANSFER_DATA (0xFF01)
#define SERIAL_TRANSFER_SERIAL_CONTROL (0xFF02)

//special bits
#define SERIAL_CONTROL_TRANSFER_BIT (0x80)
#define SERIAL_CONTROL_CLOCKTYPE_BIT (0x01)
#define TIMER_TAC_ENABLE_BIT (0x04)
#define TIMER_TAC_CLOCKSELECT_BITS (0x03)

//global var
volatile static unsigned short IO_TIMER_CLOCKSELECT[4] = {1024, 16, 64, 256};

//main function
//check if there is data to be output on the serial bus
void perform_serialOperation(RAM* RAM_ptr);

//update the timers
void perform_timerOperation(RAM* RAM_ptr, unsigned short SystemCounter, unsigned short* LastSystemCounter_ptr, unsigned short* TimerStep_ptr);
