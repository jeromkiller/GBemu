#ifndef _SERIAL_PRINTER_H
#define _SERIAL_PRINTER_H                                        

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

//helper functions
//check if a serial opperation is requested
unsigned char check_transferStart(RAM* RAM_ptr);

//check if the internal clock has to be used
unsigned char use_internalClock(RAM* RAM_ptr);

//print the serial data
void print_serialInfo(RAM* RAM_ptr);

#endif //_SERIAL_PRINTER_H 
