// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "CPU.h"
#include "RAM.h"
#include "Opcodes.h"

#include "Tools.h"
//#include "OpcodeLookupTable.h"

int main()
{
	//startup
	RAM* GameboyRAM = RAM_init();
	CPU* GameboyCPU = CPU_init(GameboyRAM);

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	for(unsigned char i = 0; i < 20; i++)
	{
		GameboyRAM[0x0100 + i] = 0x20 + i;
	}
	
	DumpCPU(GameboyCPU);

	printf("chars\n");
	for(unsigned char i = 0; i < 10; i++)
	{
		PrintHex16bit(*Read_PC16(GameboyCPU));
		printf("\n");
	}
////////////////////
	//print data in the cpu registers
	DumpCPU(GameboyCPU);

	printf("======Msb, lsb test======\n");
	unsigned short twobyte = 4660;
	unsigned short* twobyte_ptr = &twobyte;
	unsigned char* onebyte_ptr = (unsigned char*)twobyte_ptr;
	printf("msb?: %02x\n",*onebyte_ptr & 0xff);
	printf("lsb?: %02x\n",*(onebyte_ptr + 1) & 0xff);

	DumpCPU(GameboyCPU);

	GameboyCPU->B = 0x20;
	GameboyCPU->C = 0x21;

	DumpCPU(GameboyCPU);

	PrintHex16bit(GameboyCPU->BC);

///////////////////////////////////////////////////////////////////////

	CPU_dispose(GameboyCPU);
	RAM_dispose(GameboyRAM);
    return 0;
}