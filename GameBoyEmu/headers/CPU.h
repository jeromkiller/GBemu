#ifndef _CPU_h
#define _CPU_h

//Includes
#include <stdlib.h>
#include "RAM.h"

//F register flags
/*	|   7   |  6   |   5    |   4    |3|2|1|0|
	|Zero(Z)|Sub(N)|HalfC(H)|Carry(C)|0|0|0|0|
*/
//#define FLAG_Z (CPU_ptr->Zero)
//#define FLAG_N (CPU_ptr->Subtract)
//#define FLAG_H (CPU_ptr->HCarry)
//#define FLAG_C (CPU_ptr->Carry)

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
	//Refference to the start of ram, for easier access
	RAM* RAM_ref;
}CPU;

//function prototypes
//initiate CPU
CPU* CPU_init(RAM* RAM_ptr);

//free CPU
void CPU_dispose(CPU* CPU_ptr);

//fetch the next instruction
char Read_PC(void);

//loads from the pc
unsigned char* Read_PC8(CPU* CPU_ptr);
unsigned short* Read_PC16(CPU* CPU_ptr);

#endif // !CPU