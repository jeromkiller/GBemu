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
//#define REG_AF ((short)CPU_ptr->A)
//#define REG_BC ((short)CPU_ptr->B)
//#define REG_DE ((short)CPU_ptr->D)
//#define REG_HL ((short)CPU_ptr->H)
#define REG_SP (CPU_ptr->SP)
#define REG_PC (CPU_ptr->PC)
//TODO: FIX the combined registers!


//structs
struct CPU_struct {
	char A;
	char F;
	char B;
	char C;
	char D;
	char E;
	char H;
	char L;
	short SP;
	short PC;
};

//global variables
struct CPU_struct* CPU_ptr;

//function prototypes
//initiate CPU
int CPU_init(void);

#endif // !CPU
