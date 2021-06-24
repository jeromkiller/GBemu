#pragma once

//structs
typedef struct CPU_flag_registers
{
	unsigned char null : 4;
	unsigned char Carry : 1;
	unsigned char HCarry : 1;
	unsigned char Subtract : 1;
	unsigned char Zero : 1;
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
}CPU;

//function prototypes
//initiate CPU
CPU* CPU_init();

//free CPU
void CPU_dispose(CPU* CPU_ptr);

//some getters for easy access
CPU_flags* getFlags(CPU* CPU_ptr);