#pragma once

#include "RAM.h"

typedef struct DMA_info_t DMA_info;

//initialize DMA
DMA_info* DMA_init(void);

//dispose of the DMA info
DMA_info* DMA_dispose(DMA_info* DMA_ptr);

//Perform the DMA transfer, and start the wait
void start_DMAtransfer(DMA_info* DMA_ptr, RAM* RAM_ptr, unsigned char start_address);
//lower the dma timer by cycles
void perform_DMAtick(DMA_info* DMA_ptr, unsigned char cycles);
//is DMA in progress
unsigned char get_DMArunning(DMA_info* DMA_ptr);