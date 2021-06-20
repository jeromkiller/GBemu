#pragma once

//includes
#include "RAM.h"
#include "RomMapper.h"

#include <stddef.h>
#include <stdio.h>

//defines
#define ROM_BANK_SIZE 0x4000
#define RAM_BANK_SIZE 0x2000

//enums
typedef enum Memory_controller_enum
{
    Rom_Only = 0,
    MBC1,
    MBC2,
    MBC3,
    MBC5
}Memory_controller_type;

typedef enum Memory_mode_enum
{
    ROM16_RAM8 = 0,
    ROM4_RAM32,
}Memory_mode;

//structs
//each rom bank is a linked list that links to the next bank
typedef struct Data_bank_struct
{
    unsigned char bankId;
    struct Data_bank_struct* nextBank;
    unsigned char* data;
}Data_bank;

//the memory mapper struct contains all info for the memory mapper
typedef struct Memory_Mapper_struct
{
    Memory_controller_type memory_controller;
    Memory_mode memoryMode;
    size_t rom_size;
    unsigned char num_rom_banks;
    unsigned char rom_msb;
    size_t ram_size;
    unsigned char num_ram_banks;
    unsigned char active_ram_bank;
    unsigned char ram_enabled;
    unsigned char battery;
    Data_bank* romBank_ptr;
    Data_bank* ramBank_ptr;
}Memory_Mapper;

//functions
//loads the rom and builds the mapper and initiates the rom and ram banks
Memory_Mapper* Mapper_init(char* romPath, RAM* RAM_ptr);

//get rid of the rom mapper, and free the allocated 
void Mapper_dispose(Memory_Mapper* mapper);

//use the gameboy logo as a checksum
int check_gameboyLogo(RAM* RAM_ptr);

//functions for writing to the rom bank
void write_to_rom(unsigned char* valueLocation, Memory_Mapper* mapper, RAM* RAM_ptr);
