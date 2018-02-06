// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "stdafx.h"



int main()
{
	CPU_init();
	RAM_init();

	//some user code for testing
	//insert hello world at memory location 0x0100
	char *alpha = "Hello_World\n\0";
	char *loc = LOCATION + 0x0100;
	for (int i = 0; i < 13; i++) {
		*(loc + i) = *(alpha + i);
	}
	
	//print normaly from that location
	loc = LOCATION + 0x0100;
	printf("regular printf:\n%s\n", loc);

	//print using the read pc function
	printf("walk through memmory: \n");
	for (int i = 0; i < 13; i++) {
		printf("%c", Read_PC());
	}
	printf("\n");


	//dump the CPU registers
	DumpCPU();
	

	CPU_dispose();
	RAM_dispose();
    return 0;
}

