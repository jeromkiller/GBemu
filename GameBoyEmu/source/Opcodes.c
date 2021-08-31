#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "OpcodeLookupTable.h"
#include "CPU.h"
#include "RAM.h"
#include "GameBoy.h"

#include "tools.h"

//local functions
//get a pointer to the required opcode parameter
void* getDataFromParameter(GameBoy_Instance* GB, Opcode_Parameter param);
//perform the opcode
void performOpcode(GameBoy_Instance* GB, unsigned char opcode);
//Read 8bit data from pc
unsigned char* Read_PC8(GameBoy_Instance* GB) ;
//Read 16bit data from pc
unsigned short* Read_PC16(GameBoy_Instance* GB);


//read the value in memory at the Program Counter, increase it by 1 and return the read value.
unsigned char* Read_PC8(GameBoy_Instance* GB) 
{
	return (gameboy_getRAM(GB) + (gameboy_getCPU(GB)->PC)++);
}

//read the next two bytes, increase the pc by 2
unsigned short* Read_PC16(GameBoy_Instance* GB)
{
	unsigned short* ramLocation = (unsigned short*)(gameboy_getRAM(GB) + gameboy_getCPU(GB)->PC);
	gameboy_getCPU(GB)->PC += 2;
	return ramLocation;
}

void performNextOpcode(GameBoy_Instance* GB)
{
	gameboy_getTimer(GB)->CycleDelta = 0;

	//perform the next opcode if the cpu is running
	if(gameboy_getInterruptRegs(GB)->CPU_status == CPU_RUNNING)
	{
		unsigned char opcode = *Read_PC8(GB);
		performOpcode(GB, opcode);
	}
	else
	{
		//keep the timer running anyway :P
		addCycleCount(GB, 1);
	}
}

void performOpcode(GameBoy_Instance* GB, unsigned char opcode)
{
	Instruction* instr = getNormalOpcode(opcode);

	void* param1 = getDataFromParameter(GB, instr->param1);	
	void* param2 = getDataFromParameter(GB, instr->param2);

	instr->Instruction(param1, param2, GB);
}

void* getDataFromParameter(GameBoy_Instance* GB, Opcode_Parameter param)
{
	CPU* CPU_ptr = gameboy_getCPU(GB);
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
		addCycleCount(GB, 1);
		return Read_PC8(GB);
	case IMMEDIATE_16BIT:
		addCycleCount(GB, 1);
		return Read_PC16(GB);
	//Immediate address
	case ADDRESS_8BIT:	//added to 0xFF00
	{
		addCycleCount(GB, 2);
		unsigned short address = 0xFF00 + *Read_PC8(GB);
		return gameboy_getRAM(GB) + address;
	}
	case ADDRESS_16BIT:
	{
		//LSB first, i don't know if this works right, right now...
		addCycleCount(GB, 3);
		unsigned short address = *Read_PC16(GB);
		return gameboy_getRAM(GB) + address;
	}
	case RELATIVE_8BIT:	//signed value added to PC, but i can't add the value to the pointer directly...
		addCycleCount(GB, 2);
		return Read_PC8(GB);
	//Register from Register
	case RELATIVE_REG_C:
		addCycleCount(GB, 1);
		return gameboy_getRAM(GB) + 0xFF00 + CPU_ptr->C;
	case ADDRESS_REG_BC:
		addCycleCount(GB, 1);
		return gameboy_getRAM(GB) + CPU_ptr->BC;
	case ADDRESS_REG_DE:
		addCycleCount(GB, 1);
		return gameboy_getRAM(GB) + CPU_ptr->DE;
	case ADDRESS_REG_HL:
		addCycleCount(GB, 1);
		return gameboy_getRAM(GB) + CPU_ptr->HL;
	//Load increases
	case ADDRESS_REG_HLI:
		addCycleCount(GB, 1);
		return gameboy_getRAM(GB) + CPU_ptr->HL++;
	case ADDRESS_REG_HLD:
		addCycleCount(GB, 1);
		return gameboy_getRAM(GB) + CPU_ptr->HL--;
	//Reset Vectors
	case RESET_0:
		return gameboy_getRAM(GB) + 0x00;
	case RESET_1:
		return gameboy_getRAM(GB) + 0x08;
	case RESET_2:
		return gameboy_getRAM(GB) + 0x10;
	case RESET_3:
		return gameboy_getRAM(GB) + 0x18;
	case RESET_4:
		return gameboy_getRAM(GB) + 0x20;
	case RESET_5:
		return gameboy_getRAM(GB) + 0x28;
	case RESET_6:
		return gameboy_getRAM(GB) + 0x30;
	case RESET_7:
		return gameboy_getRAM(GB) + 0x38;
	default:
		return NULL;
	};
}

void addCycleCount(GameBoy_Instance* GB, int cycles)
{
	TimerData* timer = gameboy_getTimer(GB);
	timer->CycleNumber += cycles;
	timer->CycleDelta += cycles * 4;
	timer->SystemTimer += cycles * 4;
}

void PUSH_Value(unsigned short value, CPU* CPU_ptr, RAM* RAM_ptr)
{
	CPU_ptr->SP -= 2;
	unsigned short* StackLocation = (unsigned short*)(RAM_ptr + CPU_ptr->SP);
	*StackLocation = value;
}

unsigned short POP_Value(CPU* CPU_ptr, RAM* RAM_ptr)
{
	unsigned short* StackLocation = (unsigned short*)(RAM_ptr + CPU_ptr->SP);
	CPU_ptr->SP += 2;
	return *StackLocation;
}

//functions for normal opcodes
//cpu instructions
//Add Value2 to Value1 (Always A) and add Carry;
void OP_ADC(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;
	unsigned char carry = flags->Carry?1:0;	//a set carry comes out as 255 for some reason

	unsigned short result = *val1 + (*val2 + carry);
	//check if the first byte of the result is zero
	flags->Zero = (result & 0xff)? 0:1;
	flags->Subtract = 0;
	//check if the low nibbles added together cause an overflow to bit 4
	flags->HCarry = (((*val1 & 0xf) + ((*val2 & 0xf) + carry)) & 0x10)? 1:0;
	//check if the result rolled over into the second byte
	flags->Carry = (result & 0x0100)? 1:0;

	*val1 = result;

	addCycleCount(GB, 1);
}

//Add Value2 to Value1
void OP_ADD16(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned int result = 0;
	unsigned short* val1 = (unsigned short*)value1;

	if(value1 == &(gameboy_getCPU(GB)->SP))
	{
		signed char* val2 = (signed char*)value2;

		//Zero flag gets reset if an adition was made to the stack pointer
		//unaffected otherwise
		flags->Zero = 0;

		//The carry and half carry seem to be based on the lower byte of SP, when adding val2 as unsigned
		flags->HCarry = (((*val1 & 0x000f) + ((unsigned char)*val2 & 0x0f)) & 0x10)?1:0;
		flags->Carry = (((*val1 & 0x00ff) + ((unsigned char)*val2 & 0xff)) & 0x100)?1:0;

		result = *val1 + *val2;
	}
	else
	{
		unsigned short* val2 = (unsigned short*)value2;

		result = *val1 + *val2;

		flags->HCarry = (((*val1 & 0x0fff) + (*val2 &0x0fff)) & 0x1000)?1:0;
		flags->Carry = (result & 0x10000)?1:0;
	}

	flags->Subtract = 0;
	
	*val1 = result;

	addCycleCount(GB, 2);
}

//Add Value2 to Value1 (Always A)
void OP_ADD8(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB)); 
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;

	unsigned short result = *val1 + *val2;

	//check if the first byte of the result is zero
	flags->Zero = (result & 0xff)? 0:1;
	flags->Subtract = 0;
	//check if the low nibbles added together cause an overflow to bit 4
	flags->HCarry = (((*val1 & 0xf) + (*val2 & 0xf)) & 0x10)? 1:0;
	//check if the result rolled over into the second byte
	flags->Carry = (result & 0x0100)? 1:0;

	*val1 = result;

	addCycleCount(GB, 1);
}

//bitwise and Value2 into Value1 (always A)
void OP_AND(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* reg1 = (unsigned char*)value1;
	unsigned char* reg2 = (unsigned char*)value2;

	unsigned char result = *reg1 & *reg2;

	flags->Zero = result?0:1;
	flags->Subtract = 0;
	flags->HCarry = 1;
	flags->Carry = 0;

	*reg1 = result;

	addCycleCount(GB, 1);
}

//Push return address to stack, and jump to value1
void OP_CALL(void *value1, void *value2, GameBoy_Instance* GB)
{
	unsigned short callAddress = *(unsigned short*)value1;
	unsigned short returnValue = gameboy_getCPU(GB)->PC;

	PUSH_Value(returnValue, gameboy_getCPU(GB), gameboy_getRAM(GB));

	gameboy_getCPU(GB)->PC = callAddress;
	
	addCycleCount(GB, 5);
}

//Push return address to stack, and jump to value1 if the carry is set
void OP_CALL_C(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(gameboy_getCPU(GB)->FLAGS.Carry)
	{
		OP_CALL(value1, value2, GB);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Push return address to stack, and jump to value1 if the carry is not set
void OP_CALL_NC(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(!gameboy_getCPU(GB)->FLAGS.Carry)
	{
		OP_CALL(value1, value2, GB);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Push return address to stack, and jump to value1 if the zero is not set
void OP_CALL_NZ(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(!gameboy_getCPU(GB)->FLAGS.Zero)
	{
		OP_CALL(value1, value2, GB);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Push return address to stack, and jump to value1 if the zero is set
void OP_CALL_Z(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(gameboy_getCPU(GB)->FLAGS.Zero)
	{
		OP_CALL(value1, value2, GB);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//call the CB prefixed opcode
void OP_CBpref(void *value1, void *value2, GameBoy_Instance* GB)
{
	unsigned char opcode = *Read_PC8(GB);
	CB_Instruction* CB_Op = getCBOpcode(opcode);
	unsigned char* reg = (unsigned char*)getDataFromParameter(GB, CB_Op->param);

	CB_Op->CB_Instruction(CB_Op->bit, reg, GB);
}

//Invert the carry flag
void OP_CCF(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	flags->Carry = !(flags->Carry);
	flags->Subtract = 0;
	flags->HCarry = 0;

	addCycleCount(GB, 1);
}

//Compare Value2 to Value1 (always A)
void OP_CP(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;

	flags->Zero = *val1 == *val2;
	flags->Subtract = 1;
	flags->HCarry = ((*val1 & 0x0f) < (*val2 & 0x0f));
	flags->Carry = (*val1 < *val2);

	addCycleCount(GB, 1);
}

//Flip all bits in REG_A
void OP_CPL(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	gameboy_getCPU(GB)->A = ~gameboy_getCPU(GB)->A;

	flags->Subtract = 1;
	flags->HCarry = 1;

	addCycleCount(GB, 1);
}
void OP_DAA(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char regA_value = gameboy_getCPU(GB)->A;

	if(flags->Subtract)	//last opperation was subctraction
	{
		if(flags->Carry)
		{
			regA_value -= 0x60;
			flags->Carry = 1;
		}
		if(flags->HCarry)
		{
			regA_value -= 0x06;
		}
	}
	else	//last opperation was an addition
	{
		if((flags->Carry) || (regA_value > 0x99))
		{
			regA_value += 0x60;
			flags->Carry = 1;
		}
		if((flags->HCarry) || ((regA_value & 0x0f) > 0x09))
		{
			regA_value += 0x06;
		}
	}

	//update other flags
	flags->Zero = regA_value ? 0:1;
	flags->HCarry = 0;

	gameboy_getCPU(GB)->A = regA_value;

	addCycleCount(GB, 1);
}

//Decrement Value1
void OP_DEC16(void *value1, void *value2, GameBoy_Instance* GB)
{
	(*(unsigned short*)value1)--;

	//No flags affected

	addCycleCount(GB, 2);
}

//Decrement Value1
void OP_DEC8(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* val1 = (unsigned char*)value1;

	(*val1)--;

	flags->Zero = *val1?0:1;
	flags->Subtract = 1;
	//if a borrow happend from the second nibble, then the fisrt nibble will be all ones
	flags->HCarry = (*val1 & 0x0f) == 0xf;

	//if (HL) was used then this opperation costs one extra cycle
	if(value1 == gameboy_getRAM(GB) + gameboy_getCPU(GB)->HL)
	{
		addCycleCount(GB, 2);
	}
	else
	{
		addCycleCount(GB, 1);
	}
}

//Disable interrupts
void OP_DI(void *value1, void *value2, GameBoy_Instance* GB)
{
	gameboy_getInterruptRegs(GB)->Interrupt_master_enable = INTERRUPT_DISABLED;

	addCycleCount(GB, 1);
}

//Enable interrupts
void OP_EI(void *value1, void *value2, GameBoy_Instance* GB)
{
	gameboy_getInterruptRegs(GB)->Interrupt_master_enable = INTERRUPT_REENABLE_SHEDUELED;

	addCycleCount(GB, 1);
}
void OP_ERROR(void *value1, void *value2, GameBoy_Instance* GB)
{
	printf("Error: illegalOpcode\n");
}

//Halt the cpu untill an interrupt occurs
void OP_HALT(void *value1, void *value2, GameBoy_Instance* GB)
{
	gameboy_getInterruptRegs(GB)->CPU_status = CPU_HALTED;

	addCycleCount(GB, 1);
}

//Increment Value2
void OP_INC16(void *value1, void *value2, GameBoy_Instance* GB)
{
	(*(unsigned short*)value1)++;

	//no flags affected

	addCycleCount(GB, 2);
}

//Increment Value1
void OP_INC8(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* val1 = (unsigned char*)value1;

	(*val1)++;

	flags->Zero = *val1?0:1;
	flags->Subtract = 0;
	//if a half carry happened, then the last nibble will all be zeroes
	flags->HCarry = (*val1 & 0x0f)?0:1;

	//if (HL) was used then this opperation costs one extra cycle
	if(value1 == gameboy_getRAM(GB) + gameboy_getCPU(GB)->HL)
	{
		addCycleCount(GB, 2);
	}
	else
	{
		addCycleCount(GB, 1);
	}
}

//Jump to Value1 
void OP_JP(void *value1, void *value2, GameBoy_Instance* GB)
{
	//I have to check the two bytes are the right way around
	gameboy_getCPU(GB)->PC = *(unsigned short*)value1;

	if(value1 != &(gameboy_getCPU(GB)->HL))
	{
		addCycleCount(GB, 3);
	}
}

//Jump to Value1 if the carry flag is set
void OP_JP_C(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(gameboy_getCPU(GB)->FLAGS.Carry)
	{
		gameboy_getCPU(GB)->PC = *(unsigned short*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Jump to Value1 if the carry flag is not set
void OP_JP_NC(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(!gameboy_getCPU(GB)->FLAGS.Carry)
	{
		gameboy_getCPU(GB)->PC = *(unsigned short*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Jump to Value1 if the zero flag is not set
void OP_JP_NZ(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(!gameboy_getCPU(GB)->FLAGS.Zero)
	{
		gameboy_getCPU(GB)->PC = *(unsigned short*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Jump to Value1 if the zero flag is set
void OP_JP_Z(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(gameboy_getCPU(GB)->FLAGS.Zero)
	{
		gameboy_getCPU(GB)->PC = *(unsigned short*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Jump to PC + value1
void OP_JR(void *value1, void *value2, GameBoy_Instance* GB)
{
	gameboy_getCPU(GB)->PC = gameboy_getCPU(GB)->PC + *(char*)value1;
	addCycleCount(GB, 3);
}

//Jump to PC + value1 if the Carry is set
void OP_JR_C(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(gameboy_getCPU(GB)->FLAGS.Carry)
	{
		gameboy_getCPU(GB)->PC = gameboy_getCPU(GB)->PC + *(char*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Jump to PC + value1 if the Carry is not set
void OP_JR_NC(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(!gameboy_getCPU(GB)->FLAGS.Carry)
	{
		gameboy_getCPU(GB)->PC = gameboy_getCPU(GB)->PC + *(char*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Jump to PC + value1 if the Zero is not set
void OP_JR_NZ(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(!gameboy_getCPU(GB)->FLAGS.Zero)
	{
		gameboy_getCPU(GB)->PC = gameboy_getCPU(GB)->PC + *(char*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Jump to PC + value1 if the Zero is set
void OP_JR_Z(void *value1, void *value2, GameBoy_Instance* GB)
{	
	if(gameboy_getCPU(GB)->FLAGS.Zero)
	{
		gameboy_getCPU(GB)->PC = gameboy_getCPU(GB)->PC + *(char*)value1;
		addCycleCount(GB, 3);
	}
	else
	{
		addCycleCount(GB, 2);
	}
}

//Load Value2, into Value1
void OP_LD16(void *value1, void *value2, GameBoy_Instance* GB)
{ 
	unsigned short* val1 = (unsigned short*)value1;
	unsigned short* val2 = (unsigned short*)value2;

	*val1 = *val2;

	addCycleCount(GB, 2);
}

//Load Value2, into Value1
void OP_LD8(void *value1, void *value2, GameBoy_Instance* GB)
{
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;
	addCycleCount(GB, 1);
	
	writeOperation(val1, val2, GB);
}

//Load SP + r8 into HL
void OP_LDHL(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned short* HL = (unsigned short*)value1;
	unsigned short* SP = (unsigned short*)value2;

	signed char relativeValue = *(signed char*)Read_PC8(GB);
	unsigned short result = *SP + relativeValue;

	flags->Zero = 0;
	flags->Subtract = 0;
	//check if the low nibbles added together cause an overflow to bit 4
	flags->HCarry = (((*SP & 0xf) + ((unsigned char)relativeValue & 0xf)) & 0x10)? 1:0;
	//check if the low and high nibbles added together cause an overflow to bit 7
	flags->Carry = (((*SP & 0xff) + ((unsigned char)relativeValue & 0xff)) & 0x100)? 1:0;

	*HL = result;

	addCycleCount(GB, 3);
}

//No Operation
void OP_NOP(void *value1, void *value2, GameBoy_Instance* GB)
{ 
	//no parameters, no flags affected
	addCycleCount(GB, 1);
}

//Bitwise OR value2 into value1 (always A)
void OP_OR(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* reg1 = (unsigned char*)value1;
	unsigned char* reg2 = (unsigned char*)value2;

	unsigned char result = *reg1 | *reg2;

	flags->Zero = result?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = 0;

	*reg1 = result;

	addCycleCount(GB, 1);
}

//Pop two bytes off the stack into a register pair
void OP_POP(void *value1, void *value2, GameBoy_Instance* GB)
{
	unsigned short* Register = (unsigned short*)value1;

	*Register = POP_Value(gameboy_getCPU(GB), gameboy_getRAM(GB));

	//there is a chance the empty bit fields of the flag register
	//get filled when popping a value into AF
	if(Register == &(gameboy_getCPU(GB)->AF))
	{
		gameboy_getCPU(GB)->FLAGS.null = 0;
	}

	addCycleCount(GB, 3);
}

//Push register pair nn onto the stack
void OP_PUSH(void *value1, void *value2, GameBoy_Instance* GB)
{ 
	unsigned short* Register = (unsigned short*)value1;

	PUSH_Value(*Register, gameboy_getCPU(GB), gameboy_getRAM(GB));

	addCycleCount(GB, 4);
}

//pop the return value from the stack and return to said value
void OP_RET(void *value1, void *value2, GameBoy_Instance* GB)
{
	gameboy_getCPU(GB)->PC = POP_Value(gameboy_getCPU(GB), gameboy_getRAM(GB));
	addCycleCount(GB, 2);
}

//Return and turn interupts back on
void OP_RETI(void *value1, void *value2, GameBoy_Instance* GB)
{
	OP_RET(value1, value2, GB);
	gameboy_getInterruptRegs(GB)->Interrupt_master_enable = INTERRUPT_ENABLED;
}

//return of the carry flag is set
void OP_RET_C(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(gameboy_getCPU(GB)->FLAGS.Carry)
	{
		OP_RET(value1, value2, GB);
	}
}

//return of the carry flag is not set
void OP_RET_NC(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(!gameboy_getCPU(GB)->FLAGS.Carry)
	{
		OP_RET(value1, value2, GB);
	}
}

//return of the zero flag is not set
void OP_RET_NZ(void *value1, void *value2, GameBoy_Instance* GB)
{	if(!gameboy_getCPU(GB)->FLAGS.Zero)
	{
		OP_RET(value1, value2, GB);
	}
}

//return of the zero flag is set
void OP_RET_Z(void *value1, void *value2, GameBoy_Instance* GB)
{
	if(gameboy_getCPU(GB)->FLAGS.Zero)
	{
		OP_RET(value1, value2, GB);
	}
}

//Rotate A left through carry
void OP_RLA(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU* CPU_ptr = gameboy_getCPU(GB);
	CPU_flags* flags = getFlags(CPU_ptr);
	unsigned char outbit = CPU_ptr->A & 0x80;
	unsigned char shifted = CPU_ptr->A << 1;

	if(flags->Carry)
	{
		shifted = shifted | 0x01; 
	}

	flags->Zero = 0; //shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	CPU_ptr->A = shifted;

	addCycleCount(GB, 1);
}

//Rotate A left into carry
void OP_RLCA(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU* CPU_ptr = gameboy_getCPU(GB);
	CPU_flags* flags = getFlags(CPU_ptr);
	unsigned char outbit = CPU_ptr->A & 0x80;
	unsigned char shifted = CPU_ptr->A << 1;

	if(outbit)
	{
		shifted = shifted | 0x01;
	}

	flags->Zero = 0;//shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	CPU_ptr->A = shifted;

	addCycleCount(GB, 1);
}

//Rotate A left through carry
void OP_RRA(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU* CPU_ptr = gameboy_getCPU(GB);
	CPU_flags* flags = getFlags(CPU_ptr);
	unsigned char outbit = CPU_ptr->A & 0x01;
	unsigned char shifted = CPU_ptr->A >> 1;

	if(flags->Carry)
	{
		shifted = shifted | 0x80; 
	}

	flags->Zero = 0; //shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	CPU_ptr->A = shifted;

	addCycleCount(GB, 1);
}

//Rotate A right into carry
void OP_RRCA(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU* CPU_ptr = gameboy_getCPU(GB);
	CPU_flags* flags = getFlags(CPU_ptr);
	unsigned char outbit = CPU_ptr->A & 0x01;
	unsigned char shifted = CPU_ptr->A >> 1;

	if(outbit)
	{
		shifted = shifted | 0x80;
	}

	flags->Zero = 0; //shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	CPU_ptr->A = shifted;

	addCycleCount(GB, 1);
	
}
void OP_RST(void *value1, void *value2, GameBoy_Instance* GB)
{
	unsigned short resetVector = (unsigned char*)value1 - gameboy_getRAM(GB);

	PUSH_Value(gameboy_getCPU(GB)->PC, gameboy_getCPU(GB), gameboy_getRAM(GB));

	gameboy_getCPU(GB)->PC = resetVector;
}

//Subctract with carry
void OP_SBC(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;
	unsigned char carry = flags->Carry?1:0;

	//reduce the result by 1 if the carry flag isn't set
	unsigned short result = *val1 - (*val2 + carry);

	//check if the first byte of the result is zero
	flags->Zero = (result & 0xff)? 0:1;
	flags->Subtract = 1;
	//check a carry took place from the second nibble to the first
	flags->HCarry = ((*val1 & 0x0f) < ((*val2 & 0x0f) + carry));
	//check if a carry took place into the second nibble
	flags->Carry = (*val1 < (*val2 + carry));

	*val1 = result;

	addCycleCount(GB, 1);
}

//Set the carry flag
void OP_SCF(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = 1;

	addCycleCount(GB, 1);
}

//halt the cpu and lcd untill a button is pressed
void OP_STOP(void *value1, void *value2, GameBoy_Instance* GB)
{
	gameboy_getInterruptRegs(GB)->CPU_status = CPU_STOPPED;

	//the stop opcode is actualy two bytes long (followed by a 00 (OP_NOP))
	gameboy_getCPU(GB)->PC++;

	addCycleCount(GB, 1);
}

//Subtract value2 from value1 (Always A)
void OP_SUB(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* val1 = (unsigned char*)value1;
	unsigned char* val2 = (unsigned char*)value2;

	unsigned short result = *val1 - *val2;

	//check if the first byte of the result is zero
	flags->Zero = (result & 0xff)? 0:1;
	flags->Subtract = 1;
	//check a carry took place from the second nibble to the first
	flags->HCarry = ((*val1 & 0x0f) < (*val2 & 0x0f));
	//check if a carry took place into the second nibble
	flags->Carry = (*val1 < *val2);

	*val1 = result;

	addCycleCount(GB, 1);
}

//Bitwise XOR value2 into value1 (always A)
void OP_XOR(void *value1, void *value2, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char* reg1 = (unsigned char*)value1;
	unsigned char* reg2 = (unsigned char*)value2;

	unsigned char result = *reg1 ^ *reg2;

	flags->Zero = result?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = 0;

	*reg1 = result;

	addCycleCount(GB, 1);
}

//functions for CB prefixed opcodes
//Rotate reg left
void OP_RLC(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char outbit = *reg & 0x80;
	unsigned char shifted = *reg << 1;

	if(outbit)
	{
		shifted = shifted | 0x01;
	}

	flags->Zero = shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	*reg = shifted;

	addCycleCount(GB, 2);
}

//Rotate reg right
void OP_RRC(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char outbit = *reg & 0x01;
	unsigned char shifted = *reg >> 1;

	if(outbit)
	{
		shifted = shifted | 0x80;
	}

	flags->Zero = shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	*reg = shifted;

	addCycleCount(GB, 2);
}

//Rotate reg left through carry
void OP_RL(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char outbit = *reg & 0x80;
	unsigned char shifted = *reg << 1;

	if(flags->Carry)
	{
		shifted = shifted | 0x01; 
	}

	flags->Zero = shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	*reg = shifted;

	addCycleCount(GB, 2);
}

//Rotate right through carry
void OP_RR(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char outbit = *reg & 0x01;
	unsigned char shifted = *reg >> 1;

	if(flags->Carry)
	{
		shifted = shifted | 0x80; 
	}

	flags->Zero = shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	*reg = shifted;

	addCycleCount(GB, 2);
}

//shift reg left into carry
void OP_SLA(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char outbit = *reg & 0x80;
	unsigned char shifted = *reg << 1;

	flags->Zero = shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	*reg = shifted;

	addCycleCount(GB, 2);
}

//shift reg right into carry (msb stays the same)
void OP_SRA(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char outbit = *reg & 0x01;
	unsigned char msb = *reg & 0x80;
	unsigned char shifted = *reg >> 1;

	shifted = shifted | msb;

	flags->Zero = shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	*reg = shifted;

	addCycleCount(GB, 2);
}

//shift reg right into carry
void OP_SRL(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char outbit = *reg & 0x01;
	unsigned char shifted = *reg >> 1;

	flags->Zero = shifted?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = outbit?1:0;

	*reg = shifted;

	addCycleCount(GB, 2);
}

//swap the upper and lower nibbles of reg
void OP_SWAP(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char upNibble = *reg & 0xF0;
	unsigned char lowNibble = *reg & 0x0F;

	flags->Zero = *reg?0:1;
	flags->Subtract = 0;
	flags->HCarry = 0;
	flags->Carry = 0;

	*reg = (upNibble >> 4) | (lowNibble << 4);

	addCycleCount(GB, 2);
}

//test bit in reg
void OP_BIT(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	CPU_flags* flags = getFlags(gameboy_getCPU(GB));
	unsigned char mask = 1 << bit;
	unsigned char result = *reg & mask;

	flags->Zero = result?0:1;
	flags->Subtract = 0;
	flags->HCarry = 1;

	addCycleCount(GB, 2);
}

//reset bit of reg
void OP_RES(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	unsigned char mask = ~(1 << bit);
	*reg = *reg & mask;

	addCycleCount(GB, 2);
}

//set bit of reg
void OP_SET(unsigned char bit, unsigned char* reg, GameBoy_Instance* GB)
{
	unsigned char mask = 1 << bit;
	*reg = *reg | mask;

	addCycleCount(GB, 2);
}