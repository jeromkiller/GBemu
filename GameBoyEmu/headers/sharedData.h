#pragma once

#include <gdk/gdk.h>

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

typedef enum SharedDataType_t
{
	SHARED_DATA_TYPE_NONE = 0,
	SHARED_DATA_TYPE_FRAMEBUFFER,
	SHARED_DATA_TYPE_PLAYER_INPUT,
} SharedDataType;

//data structures for sharing data between threads
typedef struct thread_Data_t
{
	GMutex data_mutex;
	SharedDataType type;
	int owner_max;
	int owner_count;
	union data_ptr_t
	{
		void* rawPointer;
		framebuffer_data* framebuffer;
		player_input_data* player_input;
	} data_ptr;
} thread_data;

typedef thread_data player_input;
typedef thread_data framebuffer;

//functions to create the shared data blocks
//create player input data
player_input* create_shared_input();

//create framebuffer data
framebuffer* create_shared_framebuffer();

//functions for freeing the shared data blocks
//free shared data
thread_data* free_shared_data(thread_data* data);

//functions for getting the data in the datablock
player_input_data* get_player_input_data(thread_data* data);

framebuffer_data* get_framebuffer_data(thread_data* data);

void relese_data(thread_data* data);
