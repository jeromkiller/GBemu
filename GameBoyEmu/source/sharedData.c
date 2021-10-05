#include "sharedData.h"
#include <string.h>
#include <stdio.h>

//private structures
typedef enum SharedDataType_t
{
	SHARED_DATA_TYPE_NONE = 0,
	SHARED_DATA_TYPE_STATUS,
	SHARED_DATA_TYPE_FRAMEBUFFER,
	SHARED_DATA_TYPE_PLAYER_INPUT,
}SharedDataType;

const static char* sharedDataTypeNames[] = {
	"None",
	"Status",
	"Framebuffer",
	"PlayerInput",
};

//data structures for sharing data between threads
struct thread_Data_Header_t
{
	GMutex data_mutex;
	SharedDataType type;
	int owner_max;
	int owner_count;
	union data_ptr_t
	{
		void* rawPointer;
		emu_status_flags_data* status_flags;
		framebuffer_data* framebuffer;
		player_input_data* player_input;
	} data_ptr;
};

struct shared_Thread_Blocks_t
{
	//shared data
	GMutex blocks_mutex;
	emu_status_flags* emu_status;
	player_input* input;
	framebuffer* fb;

	char* romfile;
};

//private function definitions
static thread_Data_Header* create_shared_data_header();
static emu_status_flags* create_shared_status_flags();
static player_input* create_shared_input();
static framebuffer* create_shared_framebuffer();
static int validate_shared_data(thread_Data_Header* data, SharedDataType type);
static void* get_shared_data_from_header(thread_Data_Header* data);

//create the header for the data block
//the creator of this block is the first owner of the shared data inside
static thread_Data_Header* create_shared_data_header()
{
	//allocate the data header for the data
	thread_Data_Header* shared_data_header = malloc(sizeof(thread_Data_Header));
	shared_data_header->owner_count = 0;
	shared_data_header->owner_max = 0;
	shared_data_header->type = SHARED_DATA_TYPE_NONE;

	//create the mutex
	g_mutex_init(&shared_data_header->data_mutex);

	return shared_data_header;
}

//create the emu_status flags
static emu_status_flags* create_shared_status_flags()
{
	//create the header for the emu_status flags dat
	emu_status_flags* shared_block = (emu_status_flags*)create_shared_data_header();
	shared_block->type = SHARED_DATA_TYPE_STATUS;
	shared_block->owner_max = 2;
	shared_block->owner_count = 1;

	//create the emu_status flags struct
	emu_status_flags_data* status_data = (emu_status_flags_data*)malloc(sizeof(emu_status_flags_data));
	memset(status_data, 0, sizeof(emu_status_flags_data));
	shared_block->data_ptr.status_flags = status_data;
	
	return shared_block;
}

static player_input* create_shared_input()
{
	//create the header for the input data
	player_input* shared_block = (player_input*)create_shared_data_header();
	shared_block->type = SHARED_DATA_TYPE_PLAYER_INPUT;
	shared_block->owner_max = 2;
	shared_block->owner_count = 1;

	//create the input data struct
	player_input_data* input_data = (player_input_data*)malloc(sizeof(player_input_data));
	memset(input_data, 0xff, sizeof(player_input_data));
	shared_block->data_ptr.player_input = input_data;

	return shared_block;
}

//create framebuffer data
static framebuffer* create_shared_framebuffer()
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

//free player input data
thread_Data_Header* free_shared_data(thread_Data_Header* data)
{
	if(NULL == data)
	{
		return NULL;
	}
	
	if(data->owner_count <= 0)
	{
		//this data should already be freed, just crash for now.
		printf("ERROR: shared data block is an orphan, and should already have been freed\n");
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
	return data;
}

//lock the data and return a pointer to the data
static void* get_shared_data_from_header(thread_Data_Header* data)
{
	//wait untill the data is accasable
	g_mutex_lock(&data->data_mutex);
	return data->data_ptr.rawPointer;
}

//functions for freeing the shared data blocks
//assign yourself as a owner of the data
void claim_thread_data(thread_Data_Header* data)
{
	if(NULL == data)
	{
		printf("ERROR: cannot claim data, as it does not exist\n");
		return;
	}

	if(data->owner_count < data->owner_max)
	{
		data->owner_count++;
	}
	else
	{
		printf("ERROR: cannot claim data, max owner count already reached");
	}
}

shared_Thread_Blocks* create_shared_Thread_Blocks(char* romfile)
{
	//create the shared data blocks
	shared_Thread_Blocks* shared_blocks = (shared_Thread_Blocks*)malloc(sizeof(shared_Thread_Blocks));
	memset(shared_blocks, 0, sizeof(shared_Thread_Blocks));
	g_mutex_init(&shared_blocks->blocks_mutex);
	shared_blocks->emu_status = create_shared_status_flags();
	shared_blocks->input = create_shared_input();
	shared_blocks->fb = create_shared_framebuffer();
	shared_blocks->romfile = romfile;
	return shared_blocks;
}
shared_Thread_Blocks* destroy_shared_Thread_Blocks(shared_Thread_Blocks* shared_data)
{
	if(NULL == shared_data)
	{
		return NULL;
	}

	g_mutex_lock(&shared_data->blocks_mutex);
	
	//free the shared data blocks
	shared_data->emu_status = free_shared_data(shared_data->emu_status);
	shared_data->input = free_shared_data(shared_data->input);
	shared_data->fb = free_shared_data(shared_data->fb);

	g_mutex_unlock(&shared_data->blocks_mutex);

	//check if we were the final owner of these blocks
	if(NULL == shared_data->emu_status &&
		NULL == shared_data->input &&
		NULL == shared_data->fb)
	{
		free(shared_data);
		shared_data = NULL;
	}

	return shared_data;
}

emu_status_flags* get_shared_status_flags(shared_Thread_Blocks* dataBlocks)
{
	if(NULL == dataBlocks)
	{
		printf("ERROR: cannot get emu_status flags, as shared data blocks are NULL\n");
		return NULL;
	}
	return dataBlocks->emu_status;
}

player_input* get_shared_player_input(shared_Thread_Blocks* dataBlocks)
{
	if(NULL == dataBlocks)
	{
		printf("ERROR: cannot get player input, as shared data blocks are NULL\n");
		return NULL;
	}
	return dataBlocks->input;
}
framebuffer* get_shared_framebuffer(shared_Thread_Blocks* dataBlocks)
{
	if(NULL == dataBlocks)
	{
		printf("ERROR: cannot get framebuffer, as shared data blocks are NULL\n");
		return NULL;
	}
	return dataBlocks->fb;
}

char* get_romfile(shared_Thread_Blocks* dataBlocks)
{
	return dataBlocks->romfile;
}

emu_status_flags_data* get_status_flags_data(thread_Data_Header* data)
{
	if(!validate_shared_data(data, SHARED_DATA_TYPE_STATUS))
	{
		return NULL;
	}
	return (emu_status_flags_data*)get_shared_data_from_header(data);
}

//locks the mutex and gives a pointer to the player_input data
player_input_data* get_player_input_data(thread_Data_Header* data)
{
	if(!validate_shared_data(data, SHARED_DATA_TYPE_PLAYER_INPUT))
	{
		return NULL;
	}

	//lock the data and return a pointer to the data
	return (player_input_data*)get_shared_data_from_header(data);
}

framebuffer_data* get_framebuffer_data(thread_Data_Header* data)
{
	if(!validate_shared_data(data, SHARED_DATA_TYPE_FRAMEBUFFER))
	{
		return NULL;
	}

	//lock the data and return a pointer to the data
	return (framebuffer_data*)get_shared_data_from_header(data);
}

void unlock_shared_data(thread_Data_Header* data)
{
	//check if data is valid
	if(NULL == data)
	{
		printf("ERROR: data is NULL and cannot be released");
	}

	//unlock the data
	g_mutex_unlock(&data->data_mutex);
}

//check if the requested data is the same as the one supplied
//returns 1 if the data is the same, 0 if it is not
static int validate_shared_data(thread_Data_Header* data, SharedDataType type)
{
	if(NULL == data)
	{
		printf("ERROR: Shared datablock does not exist\n");
		return 0;
	}

	if(data->type != type)
	{
		printf("ERROR: the supplied header %s, is not %s\n", sharedDataTypeNames[data->type], sharedDataTypeNames[type]);
		return 0;
	}

	return 1;
}