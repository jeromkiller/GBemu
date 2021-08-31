#include "Graphics.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#define GRAPHICS_VIEWPORT_HEIGHT 144
#define GRAPHICS_VIEWPORT_WIDTH 160

//LCDC bits
#define LCDC_LCD_ENABLE 128
#define LCDC_WINDOW_TILE_MAP 64
#define LCDC_WINDOW_ENABLE 32
#define LCDC_BG_WINDOW_TILE_DATA 16
#define LCDC_BG_TILE_MAP 8
#define LCDC_OBJ_SIZE 4
#define LCDC_OBJ_ENABLE 2
#define LCDC_BG_WINDOW_PRIORITY 1

#define DOTS_PER_CLOCK_CYCLE 4
#define DOTS_PER_OAM_SEARCH 80
#define DOTS_PER_LINE 456
#define DOTS_PER_FRAME 70224

#define LINE_VBLANK_START 144
#define LINE_VBLANK_END 153

typedef struct tileLine_t
{
	union
	{
		unsigned char lsByte;
		struct
		{
			unsigned char pix7_lsb : 1;
			unsigned char pix6_lsb : 1;
			unsigned char pix5_lsb : 1;
			unsigned char pix4_lsb : 1;
			unsigned char pix3_lsb : 1;
			unsigned char pix2_lsb : 1;
			unsigned char pix1_lsb : 1;
			unsigned char pix0_lsb : 1;
		};
	};

	union
	{
		unsigned char msByte;
		struct
		{
			unsigned char pix7_msb : 1;
			unsigned char pix6_msb : 1;
			unsigned char pix5_msb : 1;
			unsigned char pix4_msb : 1;
			unsigned char pix3_msb : 1;
			unsigned char pix2_msb : 1;
			unsigned char pix1_msb : 1;
			unsigned char pix0_msb : 1;
		};
	};
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

	screenMode mode;

	//TODO: sprite array for this line

	unsigned char inactiveBuffer;
	GdkPixbuf* pixbuf[2];
};

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

//put a pixel on the screen
static void putPixel(GdkPixbuf* buffer, unsigned char x, unsigned char y, unsigned char color, unsigned char palette)
{
	//calculate the color, just dmg 
	//i'm currently not using palette
	unsigned char rgbColor = 0x55 * color;
	//invert the grayscale value
	rgbColor = ~rgbColor;

	unsigned char* pixels = gdk_pixbuf_get_pixels(buffer);
	int yChord = gdk_pixbuf_get_rowstride(buffer) * y;
	int channels = gdk_pixbuf_get_n_channels(buffer);
	int xChord = channels * x;

	for(int i = 0; i < channels; i++)
	{
		if(i == 3)
		{
			//if there is an alpha channel keep it at the max
			pixels[xChord + yChord + i] = 0xff;
		}
		else
		{
			pixels[xChord + yChord + i] = rgbColor;
		}
	}
	
}

//perform oam search
//returns amount of unhandled dots
static void oamSearch(screenData* screen_ptr, RAM* RAM_ptr, int dots)
{
	//set parameters for this scanline
	screen_ptr->scrollY = RAM_ptr[RAM_LOCATION_SCY];
	screen_ptr->scrollX = RAM_ptr[RAM_LOCATION_SCX];
	screen_ptr->interruptLine = RAM_ptr[RAM_LOCATION_LYC];
	screen_ptr->winodwY = RAM_ptr[RAM_LOCATION_WINDOW_Y];
	screen_ptr->winodwX = RAM_ptr[RAM_LOCATION_WINDOW_X];

	//TODO: Sprite support
}

screenData* screenData_init()
{
	screenData* data = malloc(sizeof(screenData));
	memset(data, 0, sizeof(screenData));

	data->mode = SCREENMODE_OAM_SEARCH;

	data->pixbuf[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_VIEWPORT_WIDTH, GRAPHICS_VIEWPORT_HEIGHT);
	data->pixbuf[1] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_VIEWPORT_WIDTH, GRAPHICS_VIEWPORT_HEIGHT);

	gdk_pixbuf_fill(data->pixbuf[0], 0xffffffff);
	gdk_pixbuf_fill(data->pixbuf[0], 0xffffffff);

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
	unsigned char LCDCstatus = RAM_ptr[RAM_LOCATION_LCDC];

	//if the lcd is disabled, just return
	if(!(LCDCstatus & LCDC_LCD_ENABLE))
	{
		//should also technically black out the screen
		//TODO: do that sometime
		return;
	}

	while(dotsLeft > 0)
	{
		switch (screen_ptr->mode)
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

					//check if we entered vblank
					if(screen_ptr->lineNumber >= LINE_VBLANK_START)
					{
						screen_ptr->mode = SCREENMODE_VBLANK;
					}
					else
					{
						screen_ptr->mode = SCREENMODE_OAM_SEARCH;
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

					//check if we are at a new frame
					if(screen_ptr->lineNumber > LINE_VBLANK_END)
					{
						//restart from the start of the frame
						screen_ptr->lineNumber = 0;
						screen_ptr->mode = SCREENMODE_OAM_SEARCH;
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
					screen_ptr->mode = SCREENMODE_PLACING_PIX;
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
				unsigned char bgTileMapAdressingMode = LCDCstatus & LCDC_BG_WINDOW_TILE_DATA;
				unsigned short bgTileDataLocation = bgTileMapAdressingMode ? 0x8000 : 0x8800;
				unsigned short bgTileMapLocation = LCDCstatus & LCDC_BG_TILE_MAP ? 0x9C00 : 0x9800;
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
						tileId += 127;
					}
					tile* currentTile = getTileFromId(RAM_ptr + bgTileDataLocation, tileId);
					unsigned char pixpalette = getPalette(currentTile, (screenX + viewportX) % 8, (screenY + viewportY) % 8);
					putPixel(screen_ptr->pixbuf[0], screenX, screenY, pixpalette, 0);

					//move to the next pixel
					screenX++;
					dotsLeft--;

					//if we are at the end of the scanline
					if(screenX >= GRAPHICS_VIEWPORT_WIDTH)
					{
						//break from the loop and move to hblank
						//left over dots will be consumed by the
						screen_ptr->mode = SCREENMODE_HBLANK;
						break;
					}
				};

				screen_ptr->rowNumber = screenX;
				break;
			}
		}
	}

	//update the line number in ram
	RAM_ptr[RAM_LOCATION_LY] = screen_ptr->lineNumber;

	//testing code
	//ugly quick write to the shared data pointer
	framebuffer_data* fb_data = get_framebuffer_data(fb);
	fb_data->framebuff = screen_ptr->pixbuf[0];
	fb_data->buffer_id = fb_data->buffer_id ? 0 : 1;
	unlock_shared_data(fb);
}