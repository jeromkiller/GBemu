#pragma once
#include "RAM.h"
#include "sharedData.h"

typedef struct screenData_t screenData;

//create a new screenData struct
screenData* screenData_init();

//destroy a screenData struct
screenData* screenData_dispose(screenData* screenData);

//put some pixels on the screen
void perform_LCD_operation(screenData* screenData, RAM* RAM_ptr, framebuffer* fb, int newCycles);