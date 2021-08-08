#pragma once

#include <gdk/gdk.h>

//forward declarations
typedef struct shared_Thread_Blocks_t shared_Thread_Blocks;
typedef struct thread_Data_Header_t thread_Data_Header;
typedef thread_Data_Header emu_status_flags;
typedef thread_Data_Header player_input;
typedef thread_Data_Header framebuffer;

//emu_status flags data
#define STATUS_FLAG_EMULATOR_STOPING (1)
#define STATUS_FLAG_EMULATOR_PAUSED (2)
#define STATUS_FLAG_EMULATOR_DOUBLE_SPEED (4)

typedef unsigned int status_flags_t;
typedef struct emu_status_flags_data_t
{
	status_flags_t flags;
}emu_status_flags_data;

//player input data
typedef struct player_input_data_t
{
	unsigned char input_up;
	unsigned char input_down;
	unsigned char input_left;
	unsigned char input_right;
	unsigned char input_A;
	unsigned char input_B;
	unsigned char input_start;
	unsigned char input_select;
} player_input_data;

//framebuffer data
typedef struct framebuffer_data_t
{
	unsigned char buffer_id;
	GdkPixbuf* framebuff;
} framebuffer_data;

//create a new shared thread data structure, contains all the data needed for a thread
//caller of this function is the first owner of the data
shared_Thread_Blocks* create_shared_Thread_Blocks(void);
//free a shared thread data structure, caller of this function releases the data inside
//NOTE: the thread_data blocks are not freed if there are still owners of the data
void destroy_shared_Thread_Blocks(shared_Thread_Blocks* shared_data);

//assign yourself as a owner of the data
void claim_thread_data(thread_Data_Header* data);
//release your ownership of the data, free it if you are the last owner
void free_shared_data(thread_Data_Header* data);

//functions for setting the data in the datablock
emu_status_flags* get_shared_status_flags(shared_Thread_Blocks* dataBlocks);
player_input* get_shared_player_input(shared_Thread_Blocks* dataBlocks);
framebuffer* get_shared_framebuffer(shared_Thread_Blocks* dataBlocks);

//functions for getting the data in the datablock
//lock the emu_status flags data and return a pointer to it
emu_status_flags_data* get_status_flags_data(thread_Data_Header* data);
//lock player input data and return a pointer to it
player_input_data* get_player_input_data(thread_Data_Header* data);
//lock framebuffer data and return a pointer to it
framebuffer_data* get_framebuffer_data(thread_Data_Header* data);
//unlock shared data
void unlock_shared_data(thread_Data_Header* data);

