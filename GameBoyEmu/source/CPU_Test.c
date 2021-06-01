// GameBoyEmu.c : Defines the entry point for the console application.
//

#include "CPU.h"
#include "RAM.h"
#include "RomMapper.h"
#include "Opcodes.h"
#include "SerialPrinter.h"

//#include "OpcodeLookupTable.h"

//for checking if the cpu is spooling
unsigned short lastAddress = 0;

//returns 1 if the new address of the program counter is the same as it was when it was called last time
//like when a JR -1 happens, so the cpu just spools
unsigned char isSpooling(CPU* CPU_ptr)
{
    unsigned char spooling = lastAddress == CPU_ptr->PC;
    lastAddress = CPU_ptr->PC;

    return spooling;
}

int main(int argc, char *argv[] )
{
    //check the arguments
    if(argc != 2)
    {
        printf("Wierd ammount of parameters found, please use as: %s [rom path]\n", argv[0]);
        return 0;
    }

	char* ROM_Path = argv[1];
	//startup
	RAM* GameboyRAM = RAM_init();
	Memory_Mapper* mapper = Mapper_init(ROM_Path, GameboyRAM);
	if(NULL == mapper)
	{
        printf("Error building mapper\n");
		return 0;
	}
	
	CPU* GameboyCPU = CPU_init(GameboyRAM, mapper);

///////////////////////////////////////////////////////////////////////
	//some user code for testing
	while(!isSpooling(GameboyCPU))
	{
		performNextOpcode(GameboyCPU);
		perform_serialOperation(GameboyRAM);
		fflush(stdout);
	}	

///////////////////////////////////////////////////////////////////////

	CPU_dispose(GameboyCPU);
	Mapper_dispose(mapper);
	RAM_dispose(GameboyRAM);
    return 0;
}