#pragma once

#include "GameBoy.h"

//enums
typedef enum Opcode_Parameters
{
	NONE = 0,
	//Registers
	REG_A = 1,
	REG_B,
	REG_C,
	REG_D,
	REG_E,
	REG_F,
	REG_H,
	REG_L,
	//Combined Registers
	REG_AF,
	REG_BC,
	REG_DE,
	REG_HL,
	REG_SP,
	REG_PC,
	//Immediate data
	IMMEDIATE_8BIT,
	IMMEDIATE_16BIT,
	//Immediate address
	ADDRESS_8BIT,	//added to 0xFF00
	ADDRESS_16BIT,
	RELATIVE_8BIT,	//signed char, added to PC
	RELATIVE_STACK_8BIT,	//added to SP
	//Register from Register
	RELATIVE_REG_C,
	ADDRESS_REG_BC,
	ADDRESS_REG_DE,
	ADDRESS_REG_HL,
	//Load increases
	ADDRESS_REG_HLI,
	ADDRESS_REG_HLD,
	//Reset Vectors
	RESET_0,
	RESET_1,
	RESET_2,
	RESET_3,
	RESET_4,
	RESET_5,
	RESET_6,
	RESET_7
}Opcode_Parameter;

//structure for the opcodes
typedef struct Instruction_struct 
{
	void(*Instruction)(void *value1, void* value2, GameBoy_Instance* GB);
	Opcode_Parameter param1;
	Opcode_Parameter param2;
}Instruction;

//structure for CB prefixed opcodes
typedef struct CB_Instruction_struct
{
	void(*CB_Instruction)(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
	unsigned char bit;
	Opcode_Parameter param;
}CB_Instruction;

//get the next opcode and perform it
void performNextOpcode(GameBoy_Instance* GB);

//Adds a couple of cycles to the cycle counter
void addCycleCount(GameBoy_Instance* GB, int cycles);

//push a value to the virtual stack
void PUSH_Value(unsigned short reg, GameBoy_Instance* GB);
//pop a value from the virtual stack
unsigned short POP_Value(GameBoy_Instance* GB);

//functions for normal opcodes
void OP_ADC(void *value1, void *value2, GameBoy_Instance* GB);
void OP_ADD16(void *value1, void *value2, GameBoy_Instance* GB);
void OP_ADD8(void *value1, void *value2, GameBoy_Instance* GB);
void OP_AND(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CALL(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CALL_C(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CALL_NC(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CALL_NZ(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CALL_Z(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CBpref(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CCF(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CP(void *value1, void *value2, GameBoy_Instance* GB);
void OP_CPL(void *value1, void *value2, GameBoy_Instance* GB);
void OP_DAA(void *value1, void *value2, GameBoy_Instance* GB);
void OP_DEC16(void *value1, void *value2, GameBoy_Instance* GB);
void OP_DEC8(void *value1, void *value2, GameBoy_Instance* GB);
void OP_DI(void *value1, void *value2, GameBoy_Instance* GB);
void OP_EI(void *value1, void *value2, GameBoy_Instance* GB);
void OP_ERROR(void *value1, void *value2, GameBoy_Instance* GB);
void OP_HALT(void *value1, void *value2, GameBoy_Instance* GB);
void OP_INC16(void *value1, void *value2, GameBoy_Instance* GB);
void OP_INC8(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JP(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JP_C(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JP_NC(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JP_NZ(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JP_Z(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JR(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JR_C(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JR_NC(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JR_NZ(void *value1, void *value2, GameBoy_Instance* GB);
void OP_JR_Z(void *value1, void *value2, GameBoy_Instance* GB);
void OP_LD16(void *value1, void *value2, GameBoy_Instance* GB);
void OP_LD8(void *value1, void *value2, GameBoy_Instance* GB);
void OP_LDHL(void *value1, void *value2, GameBoy_Instance* GB);
void OP_NOP(void *value1, void *value2, GameBoy_Instance* GB);
void OP_OR(void *value1, void *value2, GameBoy_Instance* GB);
void OP_POP(void *value1, void *value2, GameBoy_Instance* GB);
void OP_PUSH(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RET(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RETI(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RET_C(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RET_NC(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RET_NZ(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RET_Z(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RLA(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RLCA(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RRA(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RRCA(void *value1, void *value2, GameBoy_Instance* GB);
void OP_RST(void *value1, void *value2, GameBoy_Instance* GB);
void OP_SBC(void *value1, void *value2, GameBoy_Instance* GB);
void OP_SCF(void *value1, void *value2, GameBoy_Instance* GB);
void OP_STOP(void *value1, void *value2, GameBoy_Instance* GB);
void OP_SUB(void *value1, void *value2, GameBoy_Instance* GB);
void OP_XOR(void *value1, void *value2, GameBoy_Instance* GB);

//functions for CB prefixed opcodes
void OP_RLC(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_RRC(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_RL(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_RR(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_SLA(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_SRA(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_SRL(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_SWAP(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_BIT(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_RES(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
void OP_SET(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB);
