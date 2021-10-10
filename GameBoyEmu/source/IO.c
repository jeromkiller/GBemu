#include "IO.h"
#include "Interrupt.h"
#include "GameBoy.h"

//other includes
#include <stdio.h>

//local functions
//check if a serial opperation is requested
unsigned char check_transferStart(RAM* RAM_ptr);
//check if the internal clock has to be used
unsigned char use_internalClock(RAM* RAM_ptr);
//print the serial data
void print_serialInfo(RAM* RAM_ptr);

static unsigned short IO_TIMER_CLOCKSELECT[4] = {1024, 16, 64, 256};

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
	interruptFlags* flags= (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
	flags->serialComplete = 1;
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
			interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
			flags->timerOverflow = 1;

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

static void set_input_bits(RAM* RAM_ptr, player_input_data* input)
{
	//check which lines are high
	unsigned char* joypadReg = RAM_ptr + RAM_LOCATION_IO_JOYPAD;
	unsigned char useDirections = (*joypadReg) & 0x10;
	unsigned char useButtons = (*joypadReg) & 0x20;

	unsigned char inputBits = 0x0f;

	if(!useButtons)
	{
		inputBits &= input->rawData;
	}
	if(!useDirections)
	{
		unsigned char test = (input->rawData >> 4);
		inputBits &= test;
	}

	*joypadReg = (*joypadReg & 0xf0) | inputBits;
}

//save a copy of the player input data
void handle_newinput(RAM* RAM_ptr, player_input_data* old_data, player_input* new_input)
{
	//get the new input data
	player_input_data new_data = *get_player_input_data(new_input);
	unlock_shared_data(new_input);

	//set the input
	set_input_bits(RAM_ptr, &new_data);

	//check if any inputs were turned on from the off state
	unsigned char buttons_changed = (~old_data->rawData) & new_data.rawData;
	
	//if so fire off an interrupt
	if(buttons_changed)
	{
		//interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
		//flags->playerInput = 1; //TODO: turn back on again
	};

	*old_data = new_data;
}

//update input
void write_to_input(RAM* RAM_ptr, player_input_data* input, unsigned char value)
{
	//clean the set bytes
	unsigned char oldVal = 0b11001111 & *(RAM_ptr + RAM_LOCATION_IO_JOYPAD);
	unsigned char newBits = 0b00110000 & value;
	*(RAM_ptr + RAM_LOCATION_IO_JOYPAD) = oldVal | newBits;

	set_input_bits(RAM_ptr, input);
}

//writes to Timer Registers
void write_to_DIV(RAM* RAM_ptr, TimerData* Timer_ptr)
{
	Timer_ptr->SystemTimer = 0;
	Timer_ptr->LastSystemTimer = 0;
	Timer_ptr->TimerStep = 0;
	*(RAM_ptr + RAM_LOCATION_IO_DIV) = 0;
}
void write_to_TAC(unsigned char Value, TimerData* Timer_ptr)
{
	Timer_ptr->TimerStep = Timer_ptr->TimerStep % IO_TIMER_CLOCKSELECT[Value & TIMER_TAC_CLOCKSELECT_BITS];
}