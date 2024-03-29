#pragma once

#include "GB_type.h"

//defines
typedef unsigned char RAM;

//defines for ram locations
#define RAM_START 0x0000
#define RAM_LOCATION_LO_RAM_START 0x0000
#define RAM_LOCATION_ROM_0_START 0x0000
#define RAM_LOCATION_ROM_0_END 0x3FFF
#define RAM_LOCATION_ROM_SWAPPABLE_START 0x4000
#define RAM_LOCATION_ROM_SWAPPABLE_END 0x7FFF
#define RAM_LOCATION_VIDEO_START 0x8000
#define RAM_LOCATION_VIDEO_END 0x9FFF
#define RAM_LOCATION_RAM_SWAPPABLE_START 0xA000
#define RAM_LOCATION_RAM_SWAPPABLE_END 0xBFFF
#define RAM_LOCATION_RAM_INTERNAL_1_START 0xC000
#define RAM_LOCATION_RAM_INTERNAL_1_END 0xDFFF
#define RAM_LOCATION_RAM_ECHO_START 0xE000
#define RAM_LOCATION_RAM_ECHO_END 0xFDFF
#define RAM_LOCATION_OAM_START 0xFE00
#define RAM_LOCATION_OAM_END 0xFE9F
#define RAM_LOCATION_UNUSABLE_1_START 0xFEA0
#define RAM_LOCATION_UNUSABLE_1_END 0xFEFF
#define RAM_LOCATION_IO_START 0xFF00
#define RAM_LOCATION_IO_JOYPAD 0xFF00
#define RAM_LOCATION_IO_DIV 0xFF04
#define RAM_LOCATION_IO_TIMA 0xFF05
#define RAM_LOCATION_IO_TMA 0xFF06
#define RAM_LOCATION_IO_TAC 0xFF07
#define RAM_LOCATION_IO_IF 0xFF0F
#define RAM_LOCATION_GRAPHICS_LCDC 0xFF40
#define RAM_LOCATION_GRAPHICS_STAT 0xFF41
#define RAM_LOCATION_GRAPHICS_SCY 0xFF42
#define RAM_LOCATION_GRAPHICS_SCX 0xFF43
#define RAM_LOCATION_GRAPHICS_LY 0xFF44
#define RAM_LOCATION_GRAPHICS_LYC 0xFF45
#define RAM_LOCATION_GRAPHICS_DMA 0xFF46
#define RAM_LOCATION_GRAPHICS_BGP 0xFF47
#define RAM_LOCATION_GRAPHICS_OBP0 0xFF48
#define RAM_LOCATION_GRAPHICS_OBP1 0xFF49
#define RAM_LOCATION_GRAPHICS_WINDOW_Y 0xFF4A
#define RAM_LOCATION_GRAPHICS_WINDOW_X 0xFF4B
#define RAM_LOCATION_IO_END 0xFF4B
#define RAM_LOCATION_UNUSABLE_2_START 0xFF4C
#define RAM_LOCATION_UNUSABLE_2_END 0xFF7F
#define RAM_LOCATION_LO_RAM_END 0xFF7F
#define RAM_LOCATION_HI_RAM_START 0xFF80
#define RAM_LOCATION_RAM_INTERNAL_2_START 0xFF80
#define RAM_LOCATION_RAM_INTERNAL_2_END 0xFFFE
#define RAM_LOCATION_HI_RAM_END 0xFFFE
#define RAM_LOCATION_INTERRUPT_ENABLE 0xFFFF
#define RAM_END 0xFFFF

//function prototypes
//initialize RAM
RAM* RAM_init(void);

//dispose of the ram stack
RAM* RAM_dispose(RAM* RAM_ptr);

//get 8bit val
unsigned char get_8bitval(void* value, GameBoy_Instance* GB);
//get 16bit val
unsigned short get_16bitval(void* value, GameBoy_Instance* GB);
//set 8bit val, Gameboy_Instance added for reads from special regs in the future
void set_8bitval(unsigned char* WriteLoc, unsigned char val, GameBoy_Instance* GB);
//set 16bit val, Gameboy_Instance added for reads from special regs in the future
void set_16bitval(void* writeloc, unsigned short val, GameBoy_Instance* GB);
//push a value to the virtual stack
void PUSH_Value(unsigned short value, GameBoy_Instance* GB);
//pop a value from the virtual stack
unsigned short POP_Value(GameBoy_Instance* GB);
