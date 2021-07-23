#pragma once

#include "sharedData.h"
#include <gdk/gdk.h>

//start the gameboy emulator process
int startGameboy(shared_Thread_Blocks* threadData_ptr);

int stopGameboy(void);