#pragma once                                     

//includes
#include "RAM.h"

//defines
//Macro's for ram locations
#define SERIAL_TRANSFER_DATA (0xFF01)
#define SERIAL_TRANSFER_SERIAL_CONTROL (0xFF02)
#define SERIAL_CONTROL_TRANSFER_BIT (0x80)
#define SERIAL_CONTROL_CLOCKTYPE_BIT (0x01)

//main function
//check if there is data to be output on the serial bus
void perform_serialOperation(RAM* RAM_ptr);
