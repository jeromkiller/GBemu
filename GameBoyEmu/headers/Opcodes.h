#ifndef _Opcodes_h
#define _Opcodes_h

#include "CPU.h"

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
	void(*Instruction)(void *value1, void* value2, CPU* CPU_ptr);
	Opcode_Parameter param1;
	Opcode_Parameter param2;
}Instruction;

//Helper functions
void performNextOpcode(CPU* CPU_ptr);

void performOpcode(CPU* CPU_ptr, unsigned char opcode);

void* getDataFromParameter(CPU* CPU_ptr, Opcode_Parameter param);
//Adds a couple of cycles to the cycle counter
void addCycleCount(CPU* CPU_ptr, int cycles);

//functions for normal opcodes
void OP_ADC(void *value1, void *value2, CPU* CPU_ptr);
void OP_ADD16(void *value1, void *value2, CPU* CPU_ptr);
void OP_ADD8(void *value1, void *value2, CPU* CPU_ptr);
void OP_AND(void *value1, void *value2, CPU* CPU_ptr);
void OP_CALL(void *value1, void *value2, CPU* CPU_ptr);
void OP_CALL_C(void *value1, void *value2, CPU* CPU_ptr);
void OP_CALL_NC(void *value1, void *value2, CPU* CPU_ptr);
void OP_CALL_NZ(void *value1, void *value2, CPU* CPU_ptr);
void OP_CALL_Z(void *value1, void *value2, CPU* CPU_ptr);
void OP_CBpref(void *value1, void *value2, CPU* CPU_ptr);
void OP_CCF(void *value1, void *value2, CPU* CPU_ptr);
void OP_CP(void *value1, void *value2, CPU* CPU_ptr);
void OP_CPL(void *value1, void *value2, CPU* CPU_ptr);
void OP_DAA(void *value1, void *value2, CPU* CPU_ptr);
void OP_DEC16(void *value1, void *value2, CPU* CPU_ptr);
void OP_DEC8(void *value1, void *value2, CPU* CPU_ptr);
void OP_DI(void *value1, void *value2, CPU* CPU_ptr);
void OP_EI(void *value1, void *value2, CPU* CPU_ptr);
void OP_ERROR(void *value1, void *value2, CPU* CPU_ptr);
void OP_HALT(void *value1, void *value2, CPU* CPU_ptr);
void OP_INC16(void *value1, void *value2, CPU* CPU_ptr);
void OP_INC8(void *value1, void *value2, CPU* CPU_ptr);
void OP_JP(void *value1, void *value2, CPU* CPU_ptr);
void OP_JP_C(void *value1, void *value2, CPU* CPU_ptr);
void OP_JP_NC(void *value1, void *value2, CPU* CPU_ptr);
void OP_JP_NZ(void *value1, void *value2, CPU* CPU_ptr);
void OP_JP_Z(void *value1, void *value2, CPU* CPU_ptr);
void OP_JR(void *value1, void *value2, CPU* CPU_ptr);
void OP_JR_C(void *value1, void *value2, CPU* CPU_ptr);
void OP_JR_NC(void *value1, void *value2, CPU* CPU_ptr);
void OP_JR_NZ(void *value1, void *value2, CPU* CPU_ptr);
void OP_JR_Z(void *value1, void *value2, CPU* CPU_ptr);
void OP_LD16(void *value1, void *value2, CPU* CPU_ptr);
void OP_LD8(void *value1, void *value2, CPU* CPU_ptr);
void OP_LDHL(void *value1, void *value2, CPU* CPU_ptr);
void OP_NOP(void *value1, void *value2, CPU* CPU_ptr);
void OP_OR(void *value1, void *value2, CPU* CPU_ptr);
void OP_POP(void *value1, void *value2, CPU* CPU_ptr);
void OP_PUSH(void *value1, void *value2, CPU* CPU_ptr);
void OP_RET(void *value1, void *value2, CPU* CPU_ptr);
void OP_RETI(void *value1, void *value2, CPU* CPU_ptr);
void OP_RET_C(void *value1, void *value2, CPU* CPU_ptr);
void OP_RET_NC(void *value1, void *value2, CPU* CPU_ptr);
void OP_RET_NZ(void *value1, void *value2, CPU* CPU_ptr);
void OP_RET_Z(void *value1, void *value2, CPU* CPU_ptr);
void OP_RLA(void *value1, void *value2, CPU* CPU_ptr);
void OP_RLCA(void *value1, void *value2, CPU* CPU_ptr);
void OP_RRA(void *value1, void *value2, CPU* CPU_ptr);
void OP_RRCA(void *value1, void *value2, CPU* CPU_ptr);
void OP_RST(void *value1, void *value2, CPU* CPU_ptr);
void OP_SBC(void *value1, void *value2, CPU* CPU_ptr);
void OP_SCF(void *value1, void *value2, CPU* CPU_ptr);
void OP_STOP(void *value1, void *value2, CPU* CPU_ptr);
void OP_SUB(void *value1, void *value2, CPU* CPU_ptr);
void OP_XOR(void *value1, void *value2, CPU* CPU_ptr);


//functions for CB prefixed opcodes


#endif // !_Opcodes_h