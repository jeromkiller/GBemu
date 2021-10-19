#include "DMA.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define OAM_SIZE 120
#define DMA_TIME 160

struct DMA_info_t
{
	unsigned short DMA_cycles;
	unsigned int DMA_running;
};

//initialize DMA
DMA_info* DMA_init(void)
{
	DMA_info* newDMA = (DMA_info*)malloc(sizeof(DMA_info));
	memset(newDMA, 0, sizeof(DMA_info));

	return newDMA;
}

//dispose of the DMA info
DMA_info* DMA_dispose(DMA_info* DMA_ptr)
{
	if(NULL != DMA_ptr)
	{
		free(DMA_ptr);
	}
	return NULL;
}

//Perform the DMA transfer, and start the wait
void start_DMAtransfer(DMA_info* DMA_ptr, RAM* RAM_ptr, unsigned char start_address)
{
	//don't start a new transfer untill the last one is done
	if(get_DMArunning(DMA_ptr))
	{
		printf("Error: cannot start new dma transfer, previous dma transfer is still running\n");
		return;
	}

	//check if the start address is valid
	if(start_address > 0xF1)
	{
		printf("Error: invalid dma start adres: %d\n", start_address);
		return;
	}

	//copy the info over to OAM
	const unsigned short DMA_Start = start_address << 8;
	memcpy(RAM_ptr + RAM_LOCATION_OAM_START, RAM_ptr + DMA_Start, OAM_SIZE);

	//OAM is running babyyyy
	DMA_ptr->DMA_running = 1;
}
//lower the dma timer by cycles
void perform_DMAtick(DMA_info* DMA_ptr, unsigned char cycles)
{
	//we don't have to count unless we are actually "performing DMA"
	if(!get_DMArunning(DMA_ptr))
	{
		return;
	}

	DMA_ptr->DMA_cycles += cycles;

	if(DMA_ptr->DMA_cycles > DMA_TIME)
	{
		DMA_ptr->DMA_cycles = 0;
		DMA_ptr->DMA_running = 0;
	}
}
//is DMA in progress
unsigned char get_DMArunning(DMA_info* DMA_ptr)
{
	return DMA_ptr->DMA_running;
}