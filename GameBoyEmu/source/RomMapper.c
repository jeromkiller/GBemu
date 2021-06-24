#include "RomMapper.h"
#include "RomHeader.h"

#include <string.h>
#include <stdlib.h>

//local functions
//build the mapper itself
Memory_Mapper* build_mapper(FILE* romFile, RAM* RAM_ptr);
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
//handle the write as a MBC1 controller
void write_to_MBC1(unsigned char writeValue, unsigned short writeLocation, Memory_Mapper* mapper, RAM* RAM_ptr);

//this function will probably have to change when saving functionality
Memory_Mapper* Mapper_init(char* romPath, RAM* RAM_ptr)
{
	//check if the CPU is already inited
	if(RAM_ptr == NULL)
	{
		printf("Mapper_init: Please Initialize Ram before the memory mapper\n");
		return NULL;
	}

	//open the rom file,
	FILE* romFile = fopen(romPath, "r");
	if(NULL == romFile)
	{
		printf("Could not open file: %s\n", romPath);
		return NULL;
	}

	//write to rom bank 0
	fread(RAM_ptr, sizeof(char), ROM_BANK_SIZE, romFile);
	if( ferror(romFile) != 0)
	{
		printf("Error reading file: %s\n", romPath);
	}

	//check if this is a gameboy cartridge
	if(check_gameboyLogo(RAM_ptr) != 0)
	{
		printf("Gameboy Logo checksum failed\n");
		return NULL;
	}

	//build the mapper
	Memory_Mapper* mapper = build_mapper(romFile ,RAM_ptr);

	//load the first rom bank
	swap_Rombank(1, mapper, RAM_ptr);

	//laod the first ram bank
	swap_Rambank(0, mapper, RAM_ptr);

	return mapper;
}

void Mapper_dispose(Memory_Mapper* mapper)
{
	//first clear the ram banks
	{
		Data_bank* currentBank = mapper->ramBank_ptr;
		while(NULL != currentBank)
		{
			Data_bank* nextBank = currentBank->nextBank;
			free(currentBank->data);
			free(currentBank);

			currentBank = nextBank;
		}
	mapper->ramBank_ptr = NULL;
	}

	{
		Data_bank* currentBank = mapper->romBank_ptr;
		while(NULL != currentBank)
		{
			Data_bank* nextBank = currentBank->nextBank;
			free(currentBank->data);
			free(currentBank);

			currentBank = nextBank;
		}
	mapper->romBank_ptr = NULL;
	}
	
	free(mapper);
}

Memory_Mapper* build_mapper(FILE* romFile, RAM* RAM_ptr)
{
	//make a memory mapper
	Memory_Mapper* mapper = (Memory_Mapper*)malloc(sizeof(Memory_Mapper));
	memset(mapper, 0, sizeof(Memory_Mapper));
	
	//what kind of memory controller 
	mapper->memory_controller = getControllerTypeFromRam(RAM_ptr);

	//set the memory values
	getRomSizeFromData(&mapper->num_rom_banks, &mapper->rom_size, RAM_ptr);
	getRamSizeFromData(&mapper->num_ram_banks, &mapper->ram_size, RAM_ptr);

	//build the memory banks
	mapper->romBank_ptr = build_RomBanks(romFile, mapper->num_rom_banks);
	mapper->ramBank_ptr = build_RamBanks(mapper->num_ram_banks);

	//return the finished mapper
	return mapper;
}

int check_gameboyLogo(RAM* RAM_ptr)
{
	static unsigned char gameboyLogo[] =
	{
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0b, 
		0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 
		0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 
		0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};
	
	return memcmp(gameboyLogo, RAM_ptr + ROM_HEADER_NINTENDO_GRAPHIC_START, sizeof(gameboyLogo));
}

Memory_controller_type getControllerTypeFromRam(RAM* RAM_ptr)
{
	unsigned char typeData = *(unsigned char*)(RAM_ptr + ROM_HEADER_CARTRIDGE_TYPE);

	switch (typeData)
	{
		case 0x00:
			return Rom_Only;
		case 0x01:
		case 0x02:
		case 0x03:
			return MBC1;
		case 0x05:
		case 0x06:
			return MBC2;
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			return MBC3;
		case 0x19:
		case 0x1A:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
			return MBC5;
		default:
			return Rom_Only;
		
	};
	return Rom_Only;
}

void getRomSizeFromData(unsigned char* num_banks, size_t* rom_size, RAM* RAM_ptr)
{
	unsigned char RomData = *(unsigned char*)(RAM_ptr + ROM_HEADER_ROM_SIZE);
	
	if(RomData <= 6)
	{
		*num_banks = 2 << RomData;
	}
	else
	{
		unsigned char additionValue = (RomData - 0x52) * 8;
		*num_banks = 72 + additionValue;
	}

	*rom_size = *num_banks * 0x4000;
}

void getRamSizeFromData(unsigned char* num_banks, size_t* ram_size, RAM* RAM_ptr)
{
	unsigned char RomData = *(unsigned char*)(RAM_ptr + ROM_HEADER_RAM_SIZE);

	switch (RomData)
	{
		case 1:
			//*num_banks = 1;
			//*ram_size = 0x800;
			//break;
		case 2:
			*num_banks = 1;
			*ram_size = RAM_BANK_SIZE;
		case 3:
			*num_banks = 4;
			*ram_size = RAM_BANK_SIZE * 4;
		case 4:
			*num_banks = 16;
			*ram_size = RAM_BANK_SIZE * 16;
	};
}

Data_bank* build_RomBanks(FILE* RomFile, unsigned char num_banks)
{
	if(num_banks == 0)
	{
		//i should theck the rom for tetris, since it doesn't use a mapper
		//does it report as 0 banks? or as 1 bank thats just always loaded
		return NULL;
	}

	//rom bank 0 is already loaded, so start from 1 (also makes searching slightly easier)
	Data_bank* firstBank = create_RomBank(RomFile, 1);
	Data_bank* prev_bank = firstBank;

	for(unsigned char i = 2; i < num_banks; i++)
	{
		//create a new bank
		Data_bank* new_bank = create_RomBank(RomFile, i);

		//check if the creation of the bank succeded
		if(NULL == new_bank)
		{
			break;
		}

		//make the previous bank point to the new one
		prev_bank->nextBank = new_bank;

		//set the prev bank up for the next loop
		prev_bank = new_bank;
	}

	return firstBank;
}

Data_bank* create_RomBank(FILE* RomFile, unsigned int bankId)
{
	//allocate a new rombank, and a bank worth of data
	Data_bank* new_bank = (Data_bank*)malloc(sizeof(Data_bank*));
	unsigned char* bankData = (unsigned char*)malloc(sizeof(unsigned char) * ROM_BANK_SIZE);
	
	//set the params for the rom bank
	new_bank->bankId = bankId;
	new_bank->data = bankData;
	new_bank->nextBank = NULL;

	//fill the databank
	fread(bankData, sizeof(char), ROM_BANK_SIZE, RomFile);
	if( ferror(RomFile) != 0)
	{
		printf("Error filling bank: %u\n", bankId);

		free(new_bank);
		free(bankData);

		return NULL;
	}

	//return the newly made rom bank
	return new_bank;
}

Data_bank* build_RamBanks(unsigned char num_banks)
{
	if(num_banks == 0)
	{
		return NULL;
	}

	Data_bank* firstBank = create_RamBank(0);
	Data_bank* prevBank = firstBank;

	for(unsigned char i = 1; i < num_banks; i++)
	{
		Data_bank* newBank = create_RamBank(i);

		prevBank->nextBank = newBank;
		prevBank = newBank;
	}

	return firstBank;
}

Data_bank* create_RamBank(unsigned int bankId)
{
	Data_bank* newBank = (Data_bank*)malloc(sizeof(RAM_BANK_SIZE));
	newBank->bankId = bankId;
	newBank->nextBank = NULL;
	newBank->data = (unsigned char*)malloc(sizeof(unsigned char) * RAM_BANK_SIZE);

	return newBank;
}

void swap_Rombank(unsigned char newBankNumber, Memory_Mapper* mapper, RAM* RAM_ptr)
{
	//find the right bank
	Data_bank* bank = find_bank(newBankNumber, mapper->romBank_ptr);

	//check if finding succeded
	if(NULL == bank)
	{
		return;
	}

	//copy the data in the rom bank to memory
	unsigned char* SwappableBank = RAM_ptr + RAM_LOCATION_ROM_SWAPPABLE_START;
	memcpy(SwappableBank, bank->data, ROM_BANK_SIZE);
}

void swap_Rambank(unsigned char newBankNumber, Memory_Mapper* mapper, RAM* RAM_ptr)
{
	//get a pointer to the start of the swappable ram bank
	unsigned char* SwappableBank = RAM_ptr + RAM_LOCATION_RAM_SWAPPABLE_START;

	//find the old bank
	Data_bank* oldBank = find_bank(mapper->active_ram_bank, mapper->ramBank_ptr);

	//check if finding succeded
	if(NULL == oldBank)
	{
		return;
	}

	//copy the contents of the switchable ram bank into the old bank
	memcpy(oldBank->data, SwappableBank, RAM_BANK_SIZE);

	//find the new bank
	Data_bank* newBank = find_bank(newBankNumber, mapper->ramBank_ptr);

	if(NULL == newBank)
	{
		return;
	}

	//copy the contents of the new bank to the switable bank segment
	memcpy(swap_Rambank, newBank->data, RAM_BANK_SIZE);
	mapper->active_ram_bank = newBankNumber;
}

Data_bank* find_bank(unsigned char bankNumber, Data_bank* startBank)
{
	Data_bank* bank = startBank;
	while(NULL != bank)
	{
		if(bank->bankId == bankNumber)
		{
			break;
		}
		bank = bank->nextBank;
	}

	if(NULL == bank)
	{
		printf("Error: could not find bank %hhu\n", bankNumber);
	}

	return bank;
}

void write_to_rom(unsigned char* valueLocation, Memory_Mapper* mapper, RAM* RAM_ptr)
{
	//check what part of rom was written to
	unsigned short romLocation = valueLocation - RAM_ptr;
	unsigned char writeValue = *valueLocation;
	
	//is the address we are writing to in rom space?
	if(romLocation > RAM_LOCATION_ROM_SWAPPABLE_END)
	{
		printf("Error: location %hu is not located in rom space'n", romLocation);
		return;
	}
	switch(mapper->memory_controller)
	{
		case Rom_Only:
			printf("Warning: memory controller is in rom only mode, writes to rom location %hu is ignored\n", romLocation);
			return;
		case MBC1:
			write_to_MBC1(writeValue, romLocation, mapper, RAM_ptr);
			break;
		case MBC2:
			//TODO: implement
			break;    
		case MBC3:
			//TODO: implement
			break;
		case MBC5:
			//TODO: implement
			break;    
	}
}

void write_to_MBC1(unsigned char writeValue, unsigned short writeLocation, Memory_Mapper* mapper, RAM* RAM_ptr)
{
	//call the right function depending on where was written to
	if((writeLocation >= 0x6000) && (writeLocation <= 0x7FFF))
	{
		//change memory mode
		mapper->memoryMode = (writeValue & 0x01)?ROM4_RAM32:ROM16_RAM8;
	}
	else if((writeLocation >= 0x2000) && (writeLocation <= 0x3FFF))
	{
		//change rom bank
		unsigned char romBank = writeValue & 0x1F;
		if(romBank == 0)
		{
			romBank = 1;
		}
		//most significant bytes get added after the truncation
		//causing bank 0x21, 0x41 and 0x61 to become inaccessable
		//i don't know if the rom files account for this, or if i can just ignore that
		//guess we'll have to find out in the future
		romBank = romBank | (mapper->rom_msb << 6);

		swap_Rombank(romBank, mapper, RAM_ptr);
	}
	else if(mapper->memoryMode == ROM4_RAM32)
	{
		if((writeLocation >= 0x4000) && (writeLocation <= 0x5FFF))
		{
			//swap ram bank
			unsigned char ramBank = writeValue & 0x03;
			swap_Rambank(ramBank, mapper, RAM_ptr);
		}
		else if((writeLocation >= 0x0000) && (writeLocation <= 0x1FFF))
		{
			//enable or disable ram operations
			mapper->ram_enabled = ((writeValue & 0x0F) == 0x0A)?0:1;
		}
	}
	else if(mapper->memoryMode == ROM16_RAM8)
	{
		if((writeLocation >= 0x4000) && (writeLocation <= 0x5FFF))
		{
			//set the two most significant bits of the rom address lines 
			mapper->rom_msb = writeValue & 0x03;
		}
	}
}