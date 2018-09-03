#pragma once
#ifndef RAM_h
#define RAM_h

//defines
//i guess add a macros for different parts of the stack?
//like a nice macro for the start of ram
//and for the resets i guess?
//and for the start of user code?
//but the thing is, we don't have to do these, since you can't realy do that, because the adresses always start at the beginning.
#define RAM_START (RAM_ptr)

//global variables
char* RAM_ptr;

//function prototypes
//initialize RAM
void RAM_init(void);

//dispose of the ram stack
void RAM_dispose(void);
#endif // !RAM
