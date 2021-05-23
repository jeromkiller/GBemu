#ifndef _ROM_MAPPER_H
#define _ROM_MAPPER_H

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
    size_t rom_size;
    unsigned char num_rom_banks;
    size_t ram_size;
    unsigned char num_ram_banks;
    unsigned char active_ram_bank;
    unsigned char battery;
    Data_bank* romBank_ptr;
    Data_bank* ramBank_ptr;
}Memory_Mapper;

//functions
//loads the rom and builds the mapper and initiates the rom and ram banks
Memory_Mapper* Mapper_init(char* romPath, RAM* RAM_ptr);

//get rid of the rom mapper, and free the allocated 
void Mapper_dispose(Memory_Mapper* mapper);

//build the mapper itself
Memory_Mapper* build_mapper(FILE* romFile, RAM* RAM_ptr);

//use the gameboy logo as a checksum
int check_gameboyLogo(RAM* RAM_ptr);

//get the memory controller type from ram
Memory_controller_type getControllerTypeFromRam(RAM* RAM_ptr);

//get the ROM size and number of banks from ram
void getRomSizeFromData(unsigned char* num_banks, size_t* ram_size, RAM* RAM_ptr);

//get the RAM size and number of banks from ram
void getRamSizeFromData(unsigned char* num_banks, size_t* ram_size, RAM* RAM_ptr);

//allocate and fill the rom banks
Data_bank* build_RomBanks(FILE* RomFile, unsigned char num_banks);

//create a single ram bank
Data_bank* create_RomBank(FILE* RomFile, unsigned int bankId);

//allocate and fill the rom banks
Data_bank* build_RamBanks(unsigned char num_banks);

//create a single (full size) ram bank
Data_bank* create_RamBank(unsigned int bankId);

//copy the contents of a rombank to ram
void swap_Rombank(unsigned char newBankNumber, Memory_Mapper* mapper, RAM* RAM_ptr);

//copy the contents of active ram into the old ram bank, and then swap a new bank back in
void swap_Rambank(unsigned char newBankNumber, Memory_Mapper* mapper, RAM* RAM_ptr);

//find a bank by id, returns NULL if bank could not be found
Data_bank* find_bank(unsigned char bankNumber, Data_bank* startBank);

#endif //_ROM_MAPPER