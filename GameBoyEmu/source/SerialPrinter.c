#include "SerialPrinter.h"

//other includes
#include <stdio.h>

//local functions
//check if a serial opperation is requested
unsigned char check_transferStart(RAM* RAM_ptr);
//check if the internal clock has to be used
unsigned char use_internalClock(RAM* RAM_ptr);
//print the serial data
void print_serialInfo(RAM* RAM_ptr);

void perform_serialOperation(RAM* RAM_ptr)
{
    //if we don't get a transfer request we don't need to do anything
    if(!check_transferStart(RAM_ptr))
    {
        return;
    }

    //I don't need the external clock bit yet, because we aren't acctually connected to anything
    //print the serial output
    print_serialInfo(RAM_ptr);

    //write the "incomming serial data"
    *(RAM_ptr + SERIAL_TRANSFER_DATA) = 0xFF; //nothing connected

    //reset the serial write bit
    *(RAM_ptr + SERIAL_TRANSFER_SERIAL_CONTROL) = *(RAM_ptr + SERIAL_TRANSFER_SERIAL_CONTROL) & ~SERIAL_CONTROL_TRANSFER_BIT;

    //TODO: shedeule interrupt after transfer is complete (when i get to implementing interrupts)
}

//helper functions
//check if a serial opperation is requested
unsigned char check_transferStart(RAM* RAM_ptr)
{
    return (*(RAM_ptr + SERIAL_TRANSFER_SERIAL_CONTROL)) & SERIAL_CONTROL_TRANSFER_BIT ? 1:0;
}

//check if the internal clock has to be used
unsigned char use_internalClock(RAM* RAM_ptr)
{
    return (*(RAM_ptr + SERIAL_TRANSFER_SERIAL_CONTROL)) & SERIAL_CONTROL_CLOCKTYPE_BIT ? 1:0;
}

//print the serial data
void print_serialInfo(RAM* RAM_ptr)
{
    char serialData = *(RAM_ptr + SERIAL_TRANSFER_DATA);
    printf("%c", serialData);
}