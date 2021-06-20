//includes
#include "GameBoy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GameBoy_Instance* gameBoy_init(char* romPath)
{
    //create the gameboy datastructure
    GameBoy_Instance* newGameBoy = (GameBoy_Instance*)malloc(sizeof(GameBoy_Instance));
    if(NULL == newGameBoy)
    {
        return NULL;
    }
    memset(newGameBoy, 0, sizeof(GameBoy_Instance));

    //create the datastrucutes required to run the gameboy
	newGameBoy->CPU_ref = CPU_init();
    newGameBoy->RAM_ref = RAM_init();
	newGameBoy->MAPPER_ref = Mapper_init(romPath, newGameBoy->RAM_ref);
	if(NULL == newGameBoy->MAPPER_ref)
	{
        gameBoy_dispose(newGameBoy);
		return NULL;
	}
	return newGameBoy;
}

void gameBoy_dispose(GameBoy_Instance* GameBoy)
{
    if(NULL != GameBoy)
    {
        //first dispose the member structs
	    CPU_dispose(GameBoy->CPU_ref);
	    Mapper_dispose(GameBoy->MAPPER_ref);
	    RAM_dispose(GameBoy->RAM_ref);

        //then free the gameboy struct itself
        free(GameBoy);
        GameBoy = NULL;
    }
}

CPU* getCPU(GameBoy_Instance* GB)
{
    return GB->CPU_ref;
}

RAM* getRAM(GameBoy_Instance* GB)
{
    return GB->RAM_ref;
}

Interrupt_registers* getInterruptRegs(GameBoy_Instance* GB)
{
    return GB->Interrupt_ref;
}

Memory_Mapper* getMemMapper(GameBoy_Instance* GB)
{
    return GB->MAPPER_ref;
}