#ifndef _CPU_h
#define _CPU_h

//Includes
#include <stdlib.h>
#include "RAM.h"
#include "RomMapper.h"

//Enums
typedef enum CPU_statuses
{
	RUNNING = 0,
	HALTED,
	STOPPED,
}CPU_status;

//structs
typedef struct CPU_flag_registers
{
	char null : 4;
	char Carry : 1;
	char HCarry : 1;
	char Subtract : 1;
	char Zero : 1;
}CPU_flags;

typedef struct CPU_struct 
{
	union 
	{
		//combined register
		unsigned short AF;
		//induvidual registers
		struct 
		{
			union
			{
				unsigned char F;

				CPU_flags FLAGS;
			};
			unsigned char A;
		};
	};
	union 
	{
		unsigned short BC;
		struct 
		{
			unsigned char C;
			unsigned char B;
		};
	};
	
	union 
	{
		unsigned short DE;
		struct 
		{
			unsigned char E;
			unsigned char D;
		};
	};

	union 
	{
		unsigned short HL;
		struct 
		{
			unsigned char L;
			unsigned char H;
		};
	};
	unsigned short SP;
	unsigned short PC;
	unsigned long CycleNumber;
	//CPU info
	CPU_status status;
	unsigned char interrupt_status;	//the use of interrupt status may be wrong, but i'll figure that out once i get to implementing interrupts

	//Refference to the start of ram, for easier access
	RAM* RAM_ref;
	Memory_Mapper* MAPPER_ref;
}CPU;

//function prototypes
//initiate CPU
CPU* CPU_init(RAM* RAM_ptr, Memory_Mapper* Mapper_ptr);

//free CPU
void CPU_dispose(CPU* CPU_ptr);

//loads from the pc
unsigned char* Read_PC8(CPU* CPU_ptr);
unsigned short* Read_PC16(CPU* CPU_ptr);

#endif // !CPU