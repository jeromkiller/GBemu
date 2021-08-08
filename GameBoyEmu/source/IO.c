#include "IO.h"
#include "Interrupt.h"

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

	//set interrupt flag
	//TODO: maybe set the timing
	interruptFlags_Data flags;
	flags.value = *(RAM_ptr + RAM_LOCATION_IO_IF);
	flags.interruptFlags.serialComplete = 1;
	*(RAM_ptr + RAM_LOCATION_IO_IF) = flags.value;
}

void perform_timerOperation(RAM* RAM_ptr, unsigned short SystemCounter, unsigned short* LastSystemCounter_ptr, unsigned short* TimerStep_ptr)
{
	//update the divider register (this always updates)
	unsigned char newDIV = *(RAM_ptr + RAM_LOCATION_IO_DIV);
	newDIV += (SystemCounter / 64);
	*(RAM_ptr + RAM_LOCATION_IO_DIV) = newDIV;

	//calculate the new timestep
	unsigned short newTimeStep = 0;
	if(SystemCounter < *LastSystemCounter_ptr)
	{
		//system counter has overflown
		newTimeStep = (*LastSystemCounter_ptr - SystemCounter) + *TimerStep_ptr;
	}
	else
	{
		newTimeStep = (SystemCounter - *LastSystemCounter_ptr) + *TimerStep_ptr;
	}

	unsigned char TimerEnable = *(RAM_ptr + RAM_LOCATION_IO_TAC) & TIMER_TAC_ENABLE_BIT;
	unsigned char ClockSelect = *(RAM_ptr + RAM_LOCATION_IO_TAC) & TIMER_TAC_CLOCKSELECT_BITS;    
	//the timer only increases if the timer is enabled
	if(TimerEnable)
	{
		//calculate new TIMA
		unsigned short newTIMA = *(RAM_ptr + RAM_LOCATION_IO_TIMA);
		unsigned short addition_val = (newTimeStep / IO_TIMER_CLOCKSELECT[ClockSelect]);
		newTIMA += addition_val;

		//check if TIMA overflowed
		if(newTIMA & 0xFF00)
		{
			//set the interrupt flag
			interruptFlags_Data flags;
			flags.value = *(RAM_ptr + RAM_LOCATION_IO_IF);
			flags.interruptFlags.timerOverflow = 1;
			*(RAM_ptr + RAM_LOCATION_IO_IF) = flags.value;

			//reset TIMA with the value of TIM
			newTIMA = (newTIMA & 0xFF) + *(RAM_ptr + RAM_LOCATION_IO_TMA);
		}

		//set the new TIMA
		*(RAM_ptr + RAM_LOCATION_IO_TIMA) = (unsigned char)newTIMA;
	}

	//we're done here, save a copy of the current system counter for next time
	*LastSystemCounter_ptr = SystemCounter;
	*TimerStep_ptr = newTimeStep % IO_TIMER_CLOCKSELECT[ClockSelect];
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
	//get the serial data
	unsigned char serialData = *(RAM_ptr + SERIAL_TRANSFER_DATA);

	//don't print extended ascii
	if(serialData < 0x7F)
	{
		//print the character
		int retval = printf("%c", serialData);
		if(retval < 0)
		{
			printf("\nError: could not print character\n");
		}
	}
}