// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "Tools.h"


int main()
{
	//small array of some opcodes
	struct DB_Opcode Opcodes[10] = {
		{ "LD", "B", "n", 6, 8 },
		{ "LD", "C", "n", 14, 8 },
		{ "LD", "D", "n", 22, 8 },
		{ "LD", "E", "n", 30, 8 },
		{ "LD", "H", "n", 38, 8 },
		{ "LD", "L", "n", 46, 8 },
		{ "LD", "H", "n", 38, 8 },
		{ "LD", "A", "B", 120, 8 },
		{ "LD", "A", "C", 121, 8 },
	};

	PrintAllOpcodes(&Opcodes[0], 10);
	
    return 0;
}

