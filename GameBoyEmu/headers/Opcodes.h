#ifndef _Opcodes_h
#define _Opcodes_h

//structure for the opcodes
struct Instruction_struct 
{
	void(*Instruction)(void *value1, void* value2);
	void *value1;
	void *value2;
};

//generate a pointer to an array of all the regular opcodes
//struct Instruction_struct* Normal_Opcodes_init(void);

//generate a pointer to an array of all the CB prefixed opcodes
//struct Instruction_struct* CB_Opcodes_init(void);

//free a list of opcodes
//void Opcodes_Dispose(struct Instruction_struct *Opcodes);


//returns the instruction of the opcodes
struct Instruction_struct get_NormalOpcode(char instruction);

//functions for normal opcodes

void _8bitADDliteral(void *value1, void *value2);

void _16bitADDliteral(void *value1, void *value2);


//functions for CB prefixed opcodes


#endif // !_Opcodes_h