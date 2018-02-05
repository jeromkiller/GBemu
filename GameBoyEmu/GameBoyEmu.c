// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "stdafx.h"



int main()
{
	CPU_init();

	REG_A = 255;
	CPU_ptr->null0 = 1;
	CPU_ptr->Zero = 1;
	
	DumpCPU();
	
    return 0;
}

