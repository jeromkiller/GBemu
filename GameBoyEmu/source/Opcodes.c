#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "OpcodeLookupTable.h"

void performNextOpcode(CPU* CPU_ptr)
{
	unsigned char opcode = *Read_PC8(CPU_ptr);
	performOpcode(CPU_ptr, opcode);
}

void performOpcode(CPU* CPU_ptr, unsigned char opcode)
{
	Instruction instr = getNormalOpcode(opcode);

	void* param1 = getDataFromParameter(CPU_ptr, instr.param1);
	void* param2 = getDataFromParameter(CPU_ptr, instr.param2);

	instr.Instruction(param1, param2, CPU_ptr);
}


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
		addCycleCount(CPU_ptr, 1);
		return Read_PC8(CPU_ptr);
	case IMMEDIATE_16BIT:
		addCycleCount(CPU_ptr, 1);
		return Read_PC16(CPU_ptr);
	//Immediate address
	case ADDRESS_8BIT:	//added to 0xFF00
	{
		addCycleCount(CPU_ptr, 2);
		unsigned short address = 0xFF00 + *Read_PC8(CPU_ptr);
		return CPU_ptr->RAM_ref + address;
	}
	case ADDRESS_16BIT:
	{
		//LSB first, i don't know if this works right, right now...
		addCycleCount(CPU_ptr, 3);
		unsigned short address = *Read_PC16(CPU_ptr);
		return CPU_ptr->RAM_ref + address;
	}
	case RELATIVE_8BIT:	//signed value added to PC, but i can't add the value to the pointer directly...
		addCycleCount(CPU_ptr, 2);
		return Read_PC8(CPU_ptr);
	//Register from Register
	case RELATIVE_REG_C:
		addCycleCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + 0xFF00 + CPU_ptr->C;
	case ADDRESS_REG_BC:
		addCycleCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->BC;
	case ADDRESS_REG_DE:
		addCycleCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->DE;
	case ADDRESS_REG_HL:
		addCycleCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->HL;
	//Load increases
	case ADDRESS_REG_HLI:
		addCycleCount(CPU_ptr, 1);
		return CPU_ptr->RAM_ref + CPU_ptr->HL++;
	case ADDRESS_REG_HLD:
		addCycleCount(CPU_ptr, 1);
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

void addCycleCount(CPU* CPU_ptr, int cycles)
{
	CPU_ptr->CycleNumber += cycles;
}

//functions for normal opcodes
//cpu instructions
//Add Value2 to Value1 (Always A) and add Carry;
void OP_ADC(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;
	unsigned char carry = CPU_ptr->FLAGS.Carry?1:0;	//a set carry comes out as 255 for some reason

	unsigned short result = *val1 + (*val2 + carry);
	//check if the first byte of the result is zero
	CPU_ptr->FLAGS.Zero = (result & 0xff)? 0:1;
	CPU_ptr->FLAGS.Subtract = 0;
	//check if the low nibbles added together cause an overflow to bit 4
	CPU_ptr->FLAGS.HCarry = (((*val1 & 0xf) + ((*val2 + carry) & 0xf)) & 0x10)? 1:0;
	//check if the result rolled over into the second byte
	CPU_ptr->FLAGS.Carry = (result & 0x0100)? 1:0;

	*val1 = result;

	addCycleCount(CPU_ptr, 1);
}

//Add Value2 to Value1
void OP_ADD16(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned short* val1 = (unsigned short*)value1;
	unsigned short* val2 = (unsigned short*)value2;
	
	unsigned int result = *val1 + *val2;

	if(value1 == &(CPU_ptr->SP))
	{
		//Zero flag gets reset if an adition was made to the stack pointer
		//unaffected otherwise
		CPU_ptr->FLAGS.Zero = 0;
	}
	CPU_ptr->FLAGS.Subtract = 0;
	CPU_ptr->FLAGS.HCarry = (((*val1 & 0x0fff) + (*val2 &0x0fff)) & 0x1000)?1:0;
	CPU_ptr->FLAGS.Carry = (result & 0x10000)?1:0;

	addCycleCount(CPU_ptr, 2);
}

//Add Value2 to Value1 (Always A)
void OP_ADD8(void *value1, void *value2, CPU* CPU_ptr){ 
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;

	unsigned short result = *val1 + *val2;

	//check if the first byte of the result is zero
	CPU_ptr->FLAGS.Zero = (result & 0xff)? 0:1;
	CPU_ptr->FLAGS.Subtract = 0;
	//check if the low nibbles added together cause an overflow to bit 4
	CPU_ptr->FLAGS.HCarry = (((*val1 & 0xf) + (*val2 & 0xf)) & 0x10)? 1:0;
	//check if the result rolled over into the second byte
	CPU_ptr->FLAGS.Carry = (result & 0x0100)? 1:0;

	*val1 = result;

	addCycleCount(CPU_ptr, 1);
}

//bitwise and Value2 into Value1 (always A)
void OP_AND(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* reg1 = (unsigned char*)value1;
	unsigned char* reg2 = (unsigned char*)value2;

	unsigned char result = *reg1 & *reg2;

	CPU_ptr->FLAGS.Zero = result?0:1;
	CPU_ptr->FLAGS.Subtract = 0;
	CPU_ptr->FLAGS.HCarry = 1;
	CPU_ptr->FLAGS.Carry = 0;

	*reg1 = result;

	addCycleCount(CPU_ptr, 1);
}
void OP_CALL(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_C(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_NC(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_NZ(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CALL_Z(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CBpref(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_CCF(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }

//Compare Value2 to Value1 (always A)
void OP_CP(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;

	CPU_ptr->FLAGS.Zero = *val1 == *val2;
	CPU_ptr->FLAGS.Subtract = 1;
	CPU_ptr->FLAGS.HCarry = !((*val1 & 0x0f) < (*val2 & 0x0f));
	CPU_ptr->FLAGS.Carry = !(*val1 < *val2);

	addCycleCount(CPU_ptr, 1);
}
void OP_CPL(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_DAA(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }

//Decrement Value1
void OP_DEC16(void *value1, void *value2, CPU* CPU_ptr)
{
	(*(unsigned short*)value1)--;

	//No flags affected

	addCycleCount(CPU_ptr, 2);
}

//Decrement Value1
void OP_DEC8(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	
	(*val1)--;

	CPU_ptr->FLAGS.Zero = *val1?0:1;
	CPU_ptr->FLAGS.Subtract = 0;
	//if a borrow happend from the second nibble, then the fisrt nibble will be all ones
	CPU_ptr->FLAGS.HCarry = (*val1 & 0x0f) == 0xf;

	//if (HL) was used then this opperation costs one extra cycle
	if(value1 == CPU_ptr->RAM_ref + CPU_ptr->HL)
	{
		addCycleCount(CPU_ptr, 2);
	}
	else
	{
		addCycleCount(CPU_ptr, 1);
	}
}
void OP_DI(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_EI(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_ERROR(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_HALT(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }

//Increment Value2
void OP_INC16(void *value1, void *value2, CPU* CPU_ptr)
{
	(*(unsigned short*)value1)++;

	//no flags affected

	addCycleCount(CPU_ptr, 2);
}

//Increment Value1
void OP_INC8(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	
	(*val1)++;

	CPU_ptr->FLAGS.Zero = *val1?0:1;
	CPU_ptr->FLAGS.Subtract = 0;
	//if a half carry happened, then the last nibble will all be zeroes
	CPU_ptr->FLAGS.HCarry = (*val1 & 0x0f)?0:1;

	//if (HL) was used then this opperation costs one extra cycle
	if(value1 == CPU_ptr->RAM_ref + CPU_ptr->HL)
	{
		addCycleCount(CPU_ptr, 2);
	}
	else
	{
		addCycleCount(CPU_ptr, 1);
	}
}
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

//Load Value2, into Value1
void OP_LD16(void *value1, void *value2, CPU* CPU_ptr)
{ 
	unsigned short* val1 = (unsigned short*)value1;
	unsigned short* val2 = (unsigned short*)value2;

	*val1 = *val2;

	addCycleCount(CPU_ptr, 2);
}

//Load Value2, into Value1
void OP_LD8(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;
	
	*val1 = *val2;

	addCycleCount(CPU_ptr, 1);
}

//Load SP + r8 into HL
void OP_LDHL(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned short* HL = (unsigned short*)value1;
	unsigned short* SP = (unsigned short*)value2;

	signed char relativeValue = *(signed char*)Read_PC8(CPU_ptr);
	unsigned short result = *SP + relativeValue;

	CPU_ptr->FLAGS.Zero = 0;
	CPU_ptr->FLAGS.Subtract = 0;
	//check if the low nibbles added together cause an overflow to bit 4
	CPU_ptr->FLAGS.HCarry = (((*SP & 0xf) + (relativeValue & 0xf)) & 0x10)? 1:0;
	//check if the low and high nibbles added together cause an overflow to bit 7
	CPU_ptr->FLAGS.Carry = (((*SP & 0xff) + (relativeValue & 0xff)) & 0x100)? 1:0;

	*HL = result;

	addCycleCount(CPU_ptr, 3);
}

//No Operation
void OP_NOP(void *value1, void *value2, CPU* CPU_ptr)
{ 
	//no parameters, no flags affected
	addCycleCount(CPU_ptr, 1);
}

//Bitwise OR value2 into value1 (always A)
void OP_OR(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* reg1 = (unsigned char*)value1;
	unsigned char* reg2 = (unsigned char*)value2;

	unsigned char result = *reg1 | *reg2;

	CPU_ptr->FLAGS.Zero = result?0:1;
	CPU_ptr->FLAGS.Subtract = 0;
	CPU_ptr->FLAGS.HCarry = 0;
	CPU_ptr->FLAGS.Carry = 0;

	*reg1 = result;

	addCycleCount(CPU_ptr, 1);
}

//Pop two bytes off the stack into a register pair
void OP_POP(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned short* Register = (unsigned short*)value1;

	CPU_ptr->SP += 2;
	unsigned short* StackLocation = (unsigned short*)(CPU_ptr->RAM_ref + CPU_ptr->SP);
	*Register = *StackLocation;

	addCycleCount(CPU_ptr, 3);
}

//Push register pair nn onto the stack
void OP_PUSH(void *value1, void *value2, CPU* CPU_ptr)
{ 
	unsigned short* Register = (unsigned short*)value1;

	unsigned short* StackLocation = (unsigned short*)(CPU_ptr->RAM_ref + CPU_ptr->SP);
	*StackLocation = *Register;
	CPU_ptr->SP -= 2;

	addCycleCount(CPU_ptr, 4);
}
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

//Subctract with carry
void OP_SBC(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;

	//reduce the result by 1 if the carry flag isn't set
	unsigned short result = *val1 - (*val2 - 1 + CPU_ptr->FLAGS.Carry);

		//check if the first byte of the result is zero
	CPU_ptr->FLAGS.Zero = (result & 0xff)? 0:1;
	CPU_ptr->FLAGS.Subtract = 1;
	//check a carry took place from the second nibble to the first
	CPU_ptr->FLAGS.HCarry = !((*val1 & 0x0f) < ((*val2 - 1 + CPU_ptr->FLAGS.Carry) & 0x0f));
	//check if a carry took place into the second nibble
	CPU_ptr->FLAGS.Carry = !(*val1 < (*val2 - 1 + CPU_ptr->FLAGS.Carry));

	addCycleCount(CPU_ptr, 1);
}
void OP_SCF(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }
void OP_STOP(void *value1, void *value2, CPU* CPU_ptr){ /*not yet implemented*/ }

//Subtract value2 from value1 (Always A)
void OP_SUB(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;

	unsigned short result = *val1 - *val2;

	//check if the first byte of the result is zero
	CPU_ptr->FLAGS.Zero = (result & 0xff)? 0:1;
	CPU_ptr->FLAGS.Subtract = 1;
	//check a carry took place from the second nibble to the first
	CPU_ptr->FLAGS.HCarry = !((*val1 & 0x0f) < (*val2 & 0x0f));
	//check if a carry took place into the second nibble
	CPU_ptr->FLAGS.Carry = !(*val1 < *val2);

	*val1 = result;

	addCycleCount(CPU_ptr, 1);
}

//Bitwise XOR value2 into value1 (always A)
void OP_XOR(void *value1, void *value2, CPU* CPU_ptr)
{
	unsigned char* reg1 = (unsigned char*)value1;
	unsigned char* reg2 = (unsigned char*)value2;

	unsigned char result = *reg1 ^ *reg2;

	CPU_ptr->FLAGS.Zero = result?0:1;
	CPU_ptr->FLAGS.Subtract = 0;
	CPU_ptr->FLAGS.HCarry = 0;
	CPU_ptr->FLAGS.Carry = 0;

	*reg1 = result;

	addCycleCount(CPU_ptr, 1);
}


//functions for CB prefixed opcodes