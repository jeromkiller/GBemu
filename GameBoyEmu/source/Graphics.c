#include "Graphics.h"
#include "Interrupt.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#define GRAPHICS_VIEWPORT_HEIGHT 144
#define GRAPHICS_VIEWPORT_WIDTH 160

#define DOTS_PER_CLOCK_CYCLE 4
#define DOTS_PER_OAM_SEARCH 80
#define DOTS_PER_LINE 456
#define DOTS_PER_FRAME 70224

#define LINE_VBLANK_START 144
#define LINE_VBLANK_END 153

#define NUM_OAM_ENTRIES 40
#define NUM_SPRITE_TABLE_ENTRIES 10

#define SPRITE_WIDTH 8
#define SPRITE_HEIGHT 8
#define SPRITE_FULLHEIGHT 16

#define SCREENMODE_HBLANK 0b00
#define SCREENMODE_VBLANK 0b01
#define SCREENMODE_OAM_SEARCH 0b10
#define SCREENMODE_PLACING_PIX 0b11

#define USECONDS_PER_FRAME 16742

typedef struct tileLine_t
{
	unsigned char lsByte;
	unsigned char msByte;
}tileLine;

typedef struct tile_t
{
	tileLine line[8];
}tile;

typedef struct STAT_t
{
	unsigned char mode :2;
	unsigned char LYC_Flag :1;
	unsigned char HBlank_Interrupt_enable :1;
	unsigned char VBlank_Interrupt_enable :1;
	unsigned char OAM_Interrupt_enable :1;
	unsigned char LYC_Interrupt_enable :1;
	unsigned char null :1;
}STAT;

typedef struct LCDC_t
{
	unsigned char BG_Window_enable :1;
	unsigned char OBJ_enable :1;
	unsigned char OBJ_size :1;
	unsigned char BG_tile_map :1;
	unsigned char BG_Window_tile_data :1;
	unsigned char Window_enable :1;
	unsigned char Window_tile_map :1;
	unsigned char LCD_enable :1;
}LCDC;

typedef struct OAM_Attributes_t
{
	unsigned char pallete_number_GBC : 3;
	unsigned char vram_bank : 1;
	unsigned char pallete_number_DMG : 1;
	unsigned char X_flip : 1;
	unsigned char Y_flip : 1;
	unsigned char BG_priority : 1;
}OAM_Attributes;

typedef struct OAM_Entry_t
{
	unsigned char Y_pos;
	unsigned char X_pos;
	unsigned char TileIndex;
	OAM_Attributes Attributes;
}OAM_Entry;

struct screenData_t
{
	int dotCounter;
	int dotCounter_line;

	unsigned char rowNumber;
	unsigned char lineNumber;
	unsigned char interruptLine;

	unsigned char scrollX;
	unsigned char scrollY;
	unsigned char windowX;
	unsigned char windowY;
	unsigned char windowLineNumber;

	OAM_Entry* sprites[10];

	unsigned char inactiveBuffer;
	GdkPixbuf* pixbuf[2];
	GTimer* frameTimer;
};

typedef struct rgbColor_t
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}rgbColor;

typedef enum paletteID_t
{
	BACKGROUND_PALLETE = 0,
	OBJECT_PALETTE_0,
	OBJECT_PALETTE_1
}paletteID;

//local functions
//get the tile id from the tile map based on current scanning possition
static unsigned char getTileId(unsigned char* tileMapStart, unsigned char x, unsigned char y)
{
	//this function does currently not take window possition into account
	unsigned char tileX = x / 8;
	unsigned char tileY = y / 8;
	unsigned short tileEntry = tileX + (tileY * 32);

	return tileMapStart[tileEntry];
}

//get a tile pointer from a tile id
static tile* getTileFromId(unsigned char* tileDataStart, unsigned char tileId)
{
	return (tile*)(tileDataStart + (tileId * 16));
}

//get palet from a pixel in a tile
static unsigned char getPalette(tile *tile, unsigned char pixelX, unsigned char pixelY)
{
	unsigned char palette = 0;
	tileLine* line = &tile->line[pixelY];
	palette = (line->lsByte >> (7 - pixelX)) & 0x01;
	palette |= ((line->msByte >> (7 - pixelX)) & 0x01) << 1;
	return palette;
}

//gets the right pixel color for the color value of the background
static rgbColor getColorVal(RAM* RAM_ptr, unsigned char colorVal, paletteID palID)
{
	unsigned char val;

	unsigned short paletteLocation = 0;

	switch(palID)
	{
		case BACKGROUND_PALLETE:
			paletteLocation = RAM_LOCATION_GRAPHICS_BGP;
			break;
		case OBJECT_PALETTE_0:
			paletteLocation = RAM_LOCATION_GRAPHICS_OBP0;
			break;
		case OBJECT_PALETTE_1:
			paletteLocation = RAM_LOCATION_GRAPHICS_OBP1;
			break;
	}

	unsigned char paletteData = *(RAM_ptr + paletteLocation);

	switch (colorVal & 0x03)
	{
	case 0b00:
		val = paletteData & 0b11;
		break;
	case 0b01:
		val = (paletteData & 0b1100) >> 2;
		break;
	case 0b10:
		val = (paletteData & 0b110000) >> 4;
		break;
	case 0b11:
		val = (paletteData & 0b11000000) >> 6;
		break;
	default:
		printf("ERROR: unknown pallet used\n");
		val = 0;
		break;
	}
	
	//make the grayscale pixel
	unsigned char color = 0x55 * val;
	color = ~color;

	return (rgbColor){color, color, color};
}

//put a pixel on the screen
static void putPixel(GdkPixbuf* buffer, unsigned char x, unsigned char y, rgbColor color)
{
	unsigned char* pixels = gdk_pixbuf_get_pixels(buffer);
	int yChord = gdk_pixbuf_get_rowstride(buffer) * y;
	int channels = gdk_pixbuf_get_n_channels(buffer);
	int xChord = channels * x;

	pixels[xChord + yChord + 0] = color.r;
	pixels[xChord + yChord + 1] = color.g;
	pixels[xChord + yChord + 2] = color.b;

	if(channels == 4)
	{
		pixels[xChord + yChord + 3] = 0xff;
	}
}

//perform oam search
//returns amount of unhandled dots
static void oamSearch(screenData* screen_ptr, RAM* RAM_ptr, int dots, unsigned char DMA_running)
{
	//set parameters for this scanline
	screen_ptr->scrollY = *(RAM_ptr + RAM_LOCATION_GRAPHICS_SCY);
	screen_ptr->scrollX = *(RAM_ptr + RAM_LOCATION_GRAPHICS_SCX);
	screen_ptr->interruptLine = *(RAM_ptr + RAM_LOCATION_GRAPHICS_LYC);
	screen_ptr->windowY = *(RAM_ptr + RAM_LOCATION_GRAPHICS_WINDOW_Y);
	screen_ptr->windowX = *(RAM_ptr + RAM_LOCATION_GRAPHICS_WINDOW_X);

	//clear the oam table
	screen_ptr->sprites[0] = NULL;

	//if DMA is being performed, we cannot read OAM
	if(DMA_running)
	{
		return;
	}

	//run over the oam table and store any oam entries for this line
	LCDC* LCDControl = (LCDC*)(RAM_ptr + RAM_LOCATION_GRAPHICS_LCDC);
	unsigned char realSpriteHeight = LCDControl->OBJ_size ? SPRITE_FULLHEIGHT : SPRITE_HEIGHT;

	OAM_Entry* OAM_Table = (OAM_Entry*)(RAM_ptr + RAM_LOCATION_OAM_START);
	unsigned char OAM_List_entry = 0;
	for(unsigned char i = 0; i < NUM_OAM_ENTRIES; i++)
	{
		//check if the sprite intersects the line we are currently drawing
		if((screen_ptr->lineNumber >= (OAM_Table[i].Y_pos - SPRITE_FULLHEIGHT)) &&
		(screen_ptr->lineNumber < OAM_Table[i].Y_pos - SPRITE_FULLHEIGHT + realSpriteHeight))
		{
			screen_ptr->sprites[OAM_List_entry] = &(OAM_Table[i]);
			OAM_List_entry++;
			
			//check if our oam table is full
			if(OAM_List_entry == NUM_SPRITE_TABLE_ENTRIES)
			{
				return;
			}
		}
	}
	//Write null to the lookup table to note the end of the list
	screen_ptr->sprites[OAM_List_entry] = NULL;
}



screenData* screenData_init()
{
	screenData* data = malloc(sizeof(screenData));
	memset(data, 0, sizeof(screenData));

	data->pixbuf[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_VIEWPORT_WIDTH, GRAPHICS_VIEWPORT_HEIGHT);
	data->pixbuf[1] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_VIEWPORT_WIDTH, GRAPHICS_VIEWPORT_HEIGHT);

	gdk_pixbuf_fill(data->pixbuf[0], 0xffffffff);
	gdk_pixbuf_fill(data->pixbuf[1], 0xffffffff);

	data->frameTimer = g_timer_new();
	g_timer_start(data->frameTimer);

	return data;
}

void performSleep(GTimer* frameTimer)
{
	//TODO: the timing seems a bit off, but that might be because of wsl overhead
	g_timer_stop(frameTimer);
	gulong time_taken = 0;
	g_timer_elapsed(frameTimer, &time_taken);
	if(time_taken < USECONDS_PER_FRAME)
	{
		gulong sleepTime = USECONDS_PER_FRAME - time_taken;
		g_usleep(sleepTime);
	}
	else
	{
		printf("WARNING: This frame took longer then expected\n");
	}
	g_timer_start(frameTimer);
}

screenData* screenData_dispose(screenData* screenData)
{
	if(NULL != screenData)
	{
		g_object_unref(screenData->pixbuf[0]);
		g_object_unref(screenData->pixbuf[1]);
		g_timer_stop(screenData->frameTimer);
		g_timer_destroy(screenData->frameTimer);
		free(screenData);
	}
	return NULL;
}

void perform_LCD_operation(screenData* screen_ptr, RAM* RAM_ptr, framebuffer* fb, int newCycles, unsigned char DMA_running)
{
	int dotsLeft = newCycles * DOTS_PER_CLOCK_CYCLE;
	LCDC* LCDcontrol = (LCDC*)(RAM_ptr + RAM_LOCATION_GRAPHICS_LCDC);
	STAT* LCDstatus = (STAT*)(RAM_ptr + RAM_LOCATION_GRAPHICS_STAT);

	//if the lcd is disabled, just return
	if(!LCDcontrol->LCD_enable)
	{
		//should also technically black out the screen
		//TODO: do that sometime
		return;
	}

	while(dotsLeft > 0)
	{
		switch (LCDstatus->mode)
		{
			case SCREENMODE_HBLANK:
			{
				int newDotCounter = screen_ptr->dotCounter_line + dotsLeft;

				if(newDotCounter >= DOTS_PER_LINE)
				{
					//check how many dots we overshot
					dotsLeft = DOTS_PER_LINE - newDotCounter;

					//was the window vissible on this line
					if(LCDcontrol->BG_Window_enable && (screen_ptr->lineNumber >= screen_ptr->windowY))
					{
						screen_ptr->windowLineNumber++;
					}

					//start at the next line
					newDotCounter = 0;
					screen_ptr->lineNumber++;
					screen_ptr->rowNumber = 0;

					if(screen_ptr->lineNumber == screen_ptr->interruptLine)
					{
						//set the LY Coincidence flag
						LCDstatus->LYC_Flag = 1;

						if(LCDstatus->LYC_Interrupt_enable)
						{
							//set the interrupt flag
							interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
							flags->LCDC = 1;
						}
					}
					else
					{
						LCDstatus->LYC_Flag = 0;
					}

					//check if we entered vblank
					if(screen_ptr->lineNumber >= LINE_VBLANK_START)
					{
						LCDstatus->mode = SCREENMODE_VBLANK;
						interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
						flags->VBlank = 1;

						if(LCDstatus->VBlank_Interrupt_enable)
						{
							flags->LCDC = 1;
						}

						//swap the framebuffer around
						framebuffer_data* fb_data = get_framebuffer_data(fb);
						fb_data->framebuff = screen_ptr->pixbuf[screen_ptr->inactiveBuffer];
						fb_data->buffer_id = screen_ptr->inactiveBuffer;
						screen_ptr->inactiveBuffer = screen_ptr->inactiveBuffer ? 0 : 1;
						unlock_shared_data(fb);
					}
					else
					{
						LCDstatus->mode = SCREENMODE_OAM_SEARCH;
						if(LCDstatus->OAM_Interrupt_enable)
						{
							interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
							flags->LCDC = 1;
						}
					}
				}
				else
				{
					dotsLeft = 0;
				}

				screen_ptr->dotCounter_line = newDotCounter;
				break;
			}
			case SCREENMODE_VBLANK:
			{
				int newDotCounter = screen_ptr->dotCounter_line + dotsLeft;

				if(newDotCounter >= DOTS_PER_LINE)
				{
					//see how many dots we overshot
					dotsLeft = DOTS_PER_LINE - newDotCounter;

					//start at the next line
					newDotCounter = 0;
					screen_ptr->lineNumber++;
					screen_ptr->rowNumber = 0;

					if(screen_ptr->lineNumber == screen_ptr->interruptLine)
					{
						LCDstatus->LYC_Flag = 1;

						if((LCDstatus->LYC_Interrupt_enable))
						{

							interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
							flags->LCDC = 1;
						}
					}
					else
					{
						LCDstatus->LYC_Flag = 0;
					}

					//check if we are at a new frame
					if(screen_ptr->lineNumber > LINE_VBLANK_END)
					{
						//restart from the start of the frame
						screen_ptr->lineNumber = 0;
						screen_ptr->windowLineNumber = 0;
						performSleep(screen_ptr->frameTimer);
						LCDstatus->mode = SCREENMODE_OAM_SEARCH;
						if(LCDstatus->OAM_Interrupt_enable)
						{
							interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
							flags->LCDC = 1;
						}
					}//else stay in vblank
				}
				else
				{
					dotsLeft = 0;
				}

				screen_ptr->dotCounter_line = newDotCounter;
				break;
			}
			case SCREENMODE_OAM_SEARCH:
			{
				oamSearch(screen_ptr, RAM_ptr, dotsLeft, DMA_running);
				
				int newDotCounter = screen_ptr->dotCounter_line + dotsLeft;
				
				//are we at the end of oam search?
				if(newDotCounter >= DOTS_PER_OAM_SEARCH)
				{
					//see how many dots we overshot
					dotsLeft = DOTS_PER_OAM_SEARCH - newDotCounter;
					newDotCounter = DOTS_PER_OAM_SEARCH;
					LCDstatus->mode = SCREENMODE_PLACING_PIX;
				}
				else
				{
					dotsLeft = 0;
				}
				
				screen_ptr->dotCounter_line = newDotCounter;
				break;
			}
			case SCREENMODE_PLACING_PIX:
			{
				//get the required data
				unsigned char bgTileMapAdressingMode = LCDcontrol->BG_Window_tile_data;
				unsigned short bgTileDataLocation = bgTileMapAdressingMode ? 0x8000 : 0x8800;
				unsigned short bgTileMapLocation = LCDcontrol->BG_tile_map ? 0x9C00 : 0x9800;
				unsigned short windowTileMapLocation = LCDcontrol->Window_tile_map ? 0x9C00 : 0x9800;
				unsigned char screenX = screen_ptr->rowNumber;
				unsigned char screenY = screen_ptr->lineNumber;
				unsigned char windowX = screen_ptr->windowX - 7;
				unsigned char windowY = screen_ptr->windowY;
				unsigned char windowLineNumber = screen_ptr->windowLineNumber;
				unsigned char viewportX = screen_ptr->scrollX;
				unsigned char viewportY = screen_ptr->scrollY;

				//Place a pixel for every left dot
				while(dotsLeft > 0)
				{
					unsigned char pixPalette;
					//check if the window is active on this pixel
					if(LCDcontrol->Window_enable && (screenX >= windowX) && (screenY >= windowY))
					{
						//get the tile id for the window
						unsigned char tileId = getTileId(RAM_ptr + bgTileMapLocation, screenX - windowX, windowLineNumber);
						if(!bgTileMapAdressingMode)
						{
							tileId += 128;
						}

						tile* currentTile = getTileFromId(RAM_ptr + windowTileMapLocation, tileId);
						pixPalette = getPalette(currentTile, (screenX - windowX) % 8, (screenY - windowY) % 8);
					}
					else
					{
						//get the tile id for the background layer
						unsigned char tileId = getTileId(RAM_ptr + bgTileMapLocation, screenX + viewportX, screenY + viewportY);
						if(!bgTileMapAdressingMode)
						{
							tileId += 128;
						}

						tile* currentTile = getTileFromId(RAM_ptr + bgTileDataLocation, tileId);
						pixPalette = getPalette(currentTile, (screenX + viewportX) % 8, (screenY + viewportY) % 8);
					}

					//find info for the pixel, might be overwritten by sprites
					rgbColor pixColor = getColorVal(RAM_ptr, pixPalette, BACKGROUND_PALLETE);

					if(LCDcontrol->OBJ_enable)
					{
						//is there a sprite on this pixel?
						for(unsigned char slot = 0; slot < NUM_SPRITE_TABLE_ENTRIES; slot++)
						{
							unsigned char spritePixPalette = 0;
							unsigned char spriteHeight = LCDcontrol->OBJ_size ? (SPRITE_HEIGHT * 2) : SPRITE_HEIGHT;
							OAM_Entry* sprite = screen_ptr->sprites[slot];

							//was this at the end of the list?
							if(sprite == NULL)
							{
								break;
							}

							//the sprite is on this line, we figured that out during OAM search
							//but is the sprite on this row?
							if((screenX >= (sprite->X_pos - SPRITE_WIDTH) && screenX < (sprite->X_pos)))
							{
								//check if this sprite is behind the background
								if(sprite->Attributes.BG_priority && pixPalette)
								{
									break;
								}

								//get the tile for this sprite, sprites tile data is always at 0x8000
								tile* spriteTile = getTileFromId(RAM_ptr + 0x8000, sprite->TileIndex);
								unsigned char spriteXpix = screenX - (sprite->X_pos - 8);
								unsigned char spriteYpix = screenY - (sprite->Y_pos - 16);

								//mirror the sprite if neccisery
								if(sprite->Attributes.X_flip)
								{
									char mirrorPivot = spriteXpix - 4;
									mirrorPivot = (-mirrorPivot) + 3;
									spriteXpix = mirrorPivot;
								}

								if(sprite->Attributes.Y_flip)
								{
									unsigned char halfSprite = (spriteHeight / 2);
									char mirrorPivot = spriteYpix - halfSprite;
									mirrorPivot = (-mirrorPivot) + (halfSprite - 1);
									spriteYpix = mirrorPivot;
								}

								//now we can finaly check what pallete the sprite has
								spritePixPalette = getPalette(spriteTile, spriteXpix, spriteYpix);
								
								//if its not transparent, draw the pixel for this sprite
								//if it is transparent, we should check the next sprite
								if(spritePixPalette > 0)
								{
									if(sprite->Attributes.pallete_number_DMG)
									{
										pixColor = getColorVal(RAM_ptr, spritePixPalette, OBJECT_PALETTE_1);
									}
									else
									{
										pixColor = getColorVal(RAM_ptr, spritePixPalette, OBJECT_PALETTE_0);
									}
								}
							}
						}
					}

					//write the pixel to the pixbuf
					putPixel(screen_ptr->pixbuf[screen_ptr->inactiveBuffer], screenX, screenY, pixColor);

					//move to the next pixel
					screenX++;
					dotsLeft--;

					//if we are at the end of the scanline
					if(screenX >= GRAPHICS_VIEWPORT_WIDTH)
					{
						//break from the loop and move to hblank
						//left over dots will be consumed by the
						LCDstatus->mode = SCREENMODE_HBLANK;
						if(LCDstatus->HBlank_Interrupt_enable)
						{
							interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
							flags->LCDC = 1;
						}
						break;
					}
				};

				screen_ptr->rowNumber = screenX;
				break;
			}
		}
	}

	//update the line number in ram
	*(RAM_ptr + RAM_LOCATION_GRAPHICS_LY) = screen_ptr->lineNumber;
}