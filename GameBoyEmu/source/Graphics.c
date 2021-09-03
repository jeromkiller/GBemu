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

typedef struct tileLine_t
{
	unsigned char lsByte;
	unsigned char msByte;
}tileLine;

typedef struct tile_t
{
	tileLine line[8];
}tile;

typedef enum screenMode_t
{
	SCREENMODE_HBLANK = 0,
	SCREENMODE_VBLANK,
	SCREENMODE_OAM_SEARCH,
	SCREENMODE_PLACING_PIX,
}screenMode;

typedef struct STAT_t
{
		screenMode mode :2;
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

struct screenData_t
{
	int dotCounter;
	int dotCounter_line;

	unsigned char rowNumber;
	unsigned char lineNumber;
	unsigned char interruptLine;

	unsigned char scrollX;
	unsigned char scrollY;
	unsigned char winodwX;
	unsigned char winodwY;

	//TODO: sprite array for this line

	unsigned char inactiveBuffer;
	GdkPixbuf* pixbuf[2];
};

typedef struct rgbColor_t
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}rgbColor;

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
	palette = (line->lsByte >> (7 - pixelX)) & 1;
	palette |= ((line->msByte >> (7 - pixelX)) & 1) << 1;
	return palette;
}

//gets the right pixel color for the color value of the background
static rgbColor getBackgroundColorVal(RAM* RAM_ptr, unsigned char colorVal)
{
	unsigned char val;
	unsigned char paletteData = *(RAM_ptr + RAM_LOCATION_GRAPHICS_BGP);

	switch (colorVal)
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
static void oamSearch(screenData* screen_ptr, RAM* RAM_ptr, int dots)
{
	//set parameters for this scanline
	screen_ptr->scrollY = *(RAM_ptr + RAM_LOCATION_GRAPHICS_SCY);
	screen_ptr->scrollX = *(RAM_ptr + RAM_LOCATION_GRAPHICS_SCX);
	screen_ptr->interruptLine = *(RAM_ptr + RAM_LOCATION_GRAPHICS_LYC);
	screen_ptr->winodwY = *(RAM_ptr + RAM_LOCATION_GRAPHICS_WINDOW_Y);
	screen_ptr->winodwX = *(RAM_ptr + RAM_LOCATION_GRAPHICS_WINDOW_X);

	//TODO: Sprite support
}

screenData* screenData_init()
{
	screenData* data = malloc(sizeof(screenData));
	memset(data, 0, sizeof(screenData));

	//data->mode = SCREENMODE_OAM_SEARCH;

	data->pixbuf[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_VIEWPORT_WIDTH, GRAPHICS_VIEWPORT_HEIGHT);
	data->pixbuf[1] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_VIEWPORT_WIDTH, GRAPHICS_VIEWPORT_HEIGHT);

	gdk_pixbuf_fill(data->pixbuf[0], 0xffffffff);
	gdk_pixbuf_fill(data->pixbuf[1], 0xffffffff);

	return data;
}

void screenData_dispose(screenData* screenData)
{
	g_object_unref(screenData->pixbuf[0]);
	g_object_unref(screenData->pixbuf[1]);
	free(screenData);
}

void perform_LCD_operation(screenData* screen_ptr, RAM* RAM_ptr, framebuffer* fb, int newCycles)
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

					//start at the next line
					newDotCounter = 0;
					screen_ptr->lineNumber++;
					screen_ptr->rowNumber = 0;

					if((LCDstatus->LYC_Interrupt_enable) &&
					(screen_ptr->lineNumber == screen_ptr->interruptLine))
					{
						interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
						flags->LCDC = 1;
					}

					//check if we entered vblank
					if(screen_ptr->lineNumber >= LINE_VBLANK_START)
					{
						LCDstatus->mode = SCREENMODE_VBLANK;
						interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
						flags->VBlank = 1;

						if((LCDstatus->LYC_Interrupt_enable) ||
						(LCDstatus->OAM_Interrupt_enable))
						{
							flags->LCDC = 1;
						}
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

					if((LCDstatus->LYC_Interrupt_enable) &&
					(screen_ptr->lineNumber == screen_ptr->interruptLine))
					{
						interruptFlags* flags = (interruptFlags*)(RAM_ptr + RAM_LOCATION_IO_IF);
						flags->LCDC = 1;
					}

					//check if we are at a new frame
					if(screen_ptr->lineNumber > LINE_VBLANK_END)
					{
						//restart from the start of the frame
						screen_ptr->lineNumber = 0;
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
				oamSearch(screen_ptr, RAM_ptr, dotsLeft);
				
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
				unsigned char screenX = screen_ptr->rowNumber;
				unsigned char screenY = screen_ptr->lineNumber;
				unsigned char viewportX = screen_ptr->scrollX;
				unsigned char viewportY = screen_ptr->scrollY;

				//Place a pixel for every left dot
				while(dotsLeft > 0)
				{
					//find info for the pixel, and draw it to the framebuffer
					unsigned char tileId = getTileId(RAM_ptr + bgTileMapLocation, screenX + viewportX, screenY + viewportY);
					if(!bgTileMapAdressingMode)
					{
						tileId += 128;
					}
					tile* currentTile = getTileFromId(RAM_ptr + bgTileDataLocation, tileId);
					unsigned char pixPalette = getPalette(currentTile, (screenX + viewportX) % 8, (screenY + viewportY) % 8);
					rgbColor pixColor = getBackgroundColorVal(RAM_ptr, pixPalette);
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

	//testing code
	//ugly quick write to the shared data pointer
	framebuffer_data* fb_data = get_framebuffer_data(fb);
	fb_data->framebuff = screen_ptr->pixbuf[0];
	fb_data->buffer_id = fb_data->buffer_id ? 0 : 1;
	unlock_shared_data(fb);
}