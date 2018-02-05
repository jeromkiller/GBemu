#pragma once
#ifndef CPU
#include <stdlib.h>

#define CPU


//Defines
//standard registers
#define REG_A (CPU_ptr->A)
#define REG_F (CPU_ptr->F)
#define REG_B (CPU_ptr->B)
#define REG_C (CPU_ptr->C)
#define REG_D (CPU_ptr->D)
#define REG_E (CPU_ptr->E)
#define REG_H (CPU_ptr->H)
#define REG_L (CPU_ptr->L)

//combined registers
#define REG_AF (CPU_ptr->AF)
#define REG_BC (CPU_ptr->BC)
#define REG_DE (CPU_ptr->DE)
#define REG_HL (CPU_ptr->HL)
#define REG_SP (CPU_ptr->SP)
#define REG_PC (CPU_ptr->PC)

//F register flags
/*	|   7   |  6   |   5    |   4    |3|2|1|0|
	|Zero(Z)|Sub(N)|HalfC(H)|Carry(C)|0|0|0|0|
*/
#define FLAG_Z (CPU_ptr->Zero)
#define FLAG_N (CPU_ptr->Subtract)
#define FLAG_H (CPU_ptr->HCarry)
#define FLAG_C (CPU_ptr->Carry)

//structs
struct CPU_struct {
	union {
		//combined register
		short AF;
		//induvidual registers
		struct {
			union
			{
				unsigned char F;

				struct {
					char null0 : 1;
					char null1 : 1;
					char null2 : 1;
					char null3 : 1;
					char Carry : 1;
					char HCarry : 1;
					char Subtract : 1;
					char Zero : 1;
				};
			};
			unsigned char A;
		};
	};
	union {
		short BC;
		struct {
			unsigned char C;
			unsigned char B;
		};
	};
	
	union {
		unsigned short DE;
		struct {
			unsigned char E;
			unsigned char D;
		};
	};

	union {
		unsigned short HL;
		struct {
			unsigned char L;
			unsigned char H;
		};
	};
	unsigned short SP;
	unsigned short PC;
};

//global variables
struct CPU_struct* CPU_ptr;

//function prototypes
//initiate CPU
int CPU_init(void);

#endif // !CPU
