// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "stdafx.h"



int main()
{
	CPU_init();

	REG_A = 8;
	REG_F = 3;
	REG_B = 8;
	DumpCPU();
	//prinf("%d\n" REG_AF);


    return 0;
}

