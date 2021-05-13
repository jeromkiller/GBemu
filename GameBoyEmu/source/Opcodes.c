#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "OpcodeLookupTable.h"

void* getDataFromParameter(CPU* CPU_ptr, Opcode_Parameter param)
{
	switch (param)
	{
	case NONE:
	return NULL;
	//Registers
	case REG_A:
		return &(CPU_ptr->A);
	case REG_B:
		return &(CPU_ptr->B);
	case REG_C:
		return &(CPU_ptr->C);
	case REG_D:
		return &(CPU_ptr->D);
	case REG_E:
		return &(CPU_ptr->E);
	case REG_F:
		return &(CPU_ptr->F);
	case REG_H:
		return &(CPU_ptr->H);
	case REG_L:
		return &(CPU_ptr->L);
	//Combined Registers
	case REG_AF:
		return &(CPU_ptr->AF);
	case REG_BC:
		return &(CPU_ptr->BC);
	case REG_DE:
		return &(CPU_ptr->DE);
	case REG_HL:
		return &(CPU_ptr->HL);
	case REG_SP:
		return &(CPU_ptr->SP);
	case REG_PC:
		return &(CPU_ptr->PC);
	//Immediate data
	case IMMEDIATE_8BIT:
		addCylceCount(CPU_ptr, 1);
		return Read_PC8(CPU_ptr);
	case IMMEDIATE_16BIT:
		addCylceCount(CPU_ptr, 2);
		return Read_PC16(CPU_ptr);
	//Immediate address
	case ADDRESS_8BIT:	//added to 0xFF00
	{
		addCylceCount(CPU_ptr, 2);
		unsigned short address = 0xFF00 + *Read_PC8(CPU_ptr);
		return CPU_ptr->RAM_ref + address;
	}
	case ADDRESS_16BIT:
	{
		//LSB first, i don't know if this works right, right now...
		addCylceCount(CPU_ptr, 3);
		unsigned short address = *Read_PC16(CPU_ptr);
		return CPU_ptr->RAM_ref + address;
	}
	case RELATIVE_STACK_8BIT:	//added to SP
	//Register from Register
	case RELATIVE_REG_C:
		addCylceCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + 0xFF00 + CPU_ptr->C;
	case ADDRESS_REG_BC:
		addCylceCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->BC;
	case ADDRESS_REG_DE:
		addCylceCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->DE;
	case ADDRESS_REG_HL:
		addCylceCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->HL;
	//Load increases
	case ADDRESS_REG_HLI:
		addCylceCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->HL++;
	case ADDRESS_REG_HLD:
		addCylceCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->HL--;
	//Reset Vectors
	case RESET_0:
		return CPU_ptr->RAM_ref + 0x00;
	case RESET_1:
		return CPU_ptr->RAM_ref + 0x08;
	case RESET_2:
		return CPU_ptr->RAM_ref + 0x10;
	case RESET_3:
		return CPU_ptr->RAM_ref + 0x18;
	case RESET_4:
		return CPU_ptr->RAM_ref + 0x20;
	case RESET_5:
		return CPU_ptr->RAM_ref + 0x28;
	case RESET_6:
		return CPU_ptr->RAM_ref + 0x30;
	case RESET_7:
		return CPU_ptr->RAM_ref + 0x38;
	default:
		return NULL;
	};
}

void addCylceCount(CPU* CPU_ptr, int cycles)
{
	CPU_ptr->CycleNumber += cycles;
}

//functions for normal opcodes
//cpu instructions
void OP_ADC(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_ADD16(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }

//Add Value2 to A
void OP_ADD8(void *value1, void *value2, CPU* CPU_ptr){ 
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;
	CPU_flags* flags = &(CPU_ptr->FLAGS);

	unsigned short result = *val1 + *val2;

	flags->Zero &= result == 0;
	flags->Subtract = 0;
	flags->HCarry &= result | 0x0010;
	flags->Carry &= result | 0x0100;

	*val1 = result;

	printf("Result written\n");
	fflush(stdout);

	addCylceCount(CPU_ptr, 1);
}

void OP_AND(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_C(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_NC(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_NZ(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_Z(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CBpref(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CCF(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CP(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CPL(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_DAA(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_DEC16(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_DEC8(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_DI(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_EI(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_ERROR(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_HALT(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_INC16(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_INC8(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JP(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JP_C(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JP_NC(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JP_NZ(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JP_Z(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JR(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JR_C(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JR_NC(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JR_NZ(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_JR_Z(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_LD16(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }

//Loads Value2, into Value1
void OP_LD8(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;
	
	*val1 = *val2;

	addCylceCount(CPU_ptr, 1);
}
void OP_LDH(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_LDHL(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }

void OP_NOP(void *value1, void *value2, CPU* CPU_ptr)
{ 
	addCylceCount(CPU_ptr, 1);
}
void OP_OR(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_POP(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_PUSH(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RET(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RETI(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RET_C(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RET_NC(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RET_NZ(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RET_Z(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RLA(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RLCA(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RRA(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RRCA(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_RST(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_SBC(void *value1, void *valuee2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_SCF(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_STOP(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_SUB(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_XOR(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }


//functions for CB prefixed opcodes