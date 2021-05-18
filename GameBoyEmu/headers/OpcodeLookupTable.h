#ifndef _OpcodeLookupTable_h
#define _OpcodeLookupTable_h

//includes
#include "Opcodes.h"

Instruction* getNormalOpcode(unsigned char opcode);

CB_Instruction* getCBOpcode(unsigned char opcode);

#endif