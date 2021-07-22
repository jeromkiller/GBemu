#include "sharedData.h"
#include <string.h>
#include <stdio.h>

//private functions
thread_data* create_shared_data_header();
void* get_shared_data_from_header(thread_data* data);

//the creator of this block is the first owner
player_input* create_shared_input()
{
	//create the header for the input data
	player_input* shared_block = (player_input*)create_shared_data_header();
	shared_block->type = SHARED_DATA_TYPE_PLAYER_INPUT;
	shared_block->owner_max = 2;
	shared_block->owner_count = 1;

	//create the input data struct
	player_input_data* input_data = (player_input_data*)malloc(sizeof(player_input_data));
	memset(input_data, 0, sizeof(player_input_data));
	shared_block->data_ptr.player_input = input_data;

	return shared_block;
}

//create framebuffer data
framebuffer* create_shared_framebuffer()
{
	//create the header for the framebuffer data
	framebuffer* shared_block = (framebuffer*)create_shared_data_header();
	shared_block->type = SHARED_DATA_TYPE_FRAMEBUFFER;
	shared_block->owner_max = 2;
	shared_block->owner_count = 1;

	//create the framebuffer struct
	framebuffer_data* frame_data = (framebuffer_data*)malloc(sizeof(player_input_data));
	memset(frame_data, 0, sizeof(framebuffer_data));
	shared_block->data_ptr.framebuffer = frame_data;

	return shared_block;
}

//functions for freeing the shared data blocks
//free player input data
thread_data* free_shared_data(thread_data* data)
{
	if(data->owner_count <= 0)
	{
		//this data should already be freed, just crash for now.
		printf("shared data block is an orphan, and should already have been freed\n");
		exit(0);
	}

	data->owner_count--;
	if(data->owner_count == 0)
	{
		//free the data contained in the header
		free(data->data_ptr.rawPointer);
		data->data_ptr.rawPointer = NULL;

		//free the header as well
		free(data);
		data = NULL;
	}

	return NULL;
}

//create the header for the data block
thread_data* create_shared_data_header()
{
	//allocate the data header for the data
	thread_data* shared_data_header = malloc(sizeof(thread_data));
	shared_data_header->owner_count = 0;
	shared_data_header->owner_max = 0;
	shared_data_header->type = SHARED_DATA_TYPE_NONE;

	//create the mutex
	g_mutex_init(&shared_data_header->data_mutex);

	return shared_data_header;
}

//locks the mutex and gives a pointer to the player_input data
player_input_data* get_player_input_data(thread_data* data)
{
	//check if the data is valid
	if(NULL == data)
	{
		printf("Shared datablock does not exist\n");
		return NULL;
	}

	if(data->type != SHARED_DATA_TYPE_PLAYER_INPUT)
	{
		printf("the supplied header is not for player input data\n");
		return NULL;
	}

	//lock the data and return a pointer to the data
	return (player_input_data*)get_shared_data_from_header(data);
}

framebuffer_data* get_framebuffer_data(thread_data* data)
{
	//check if the data is valid
	if(NULL == data)
	{
		printf("Shared datablock does not exist\n");
		return NULL;
	}

	if(data->type != SHARED_DATA_TYPE_FRAMEBUFFER)
	{
		printf("the supplied header is not for framebuffer data\n");
		return NULL;
	}

	//lock the data and return a pointer to the data
	return (framebuffer_data*)get_shared_data_from_header(data);
}

//lock the data and return a pointer to the data
void* get_shared_data_from_header(thread_data* data)
{
	//wait untill the data is accasable
	g_mutex_lock(&data->data_mutex);
	return data->data_ptr.rawPointer;
}

void release_data(thread_data* data)
{
	//check if data is valid
	if(NULL == data)
	{
		printf("data is NULL and cannot be released");
	}

	//unlock the data
	g_mutex_unlock(&data->data_mutex);
}