#include "Graphics.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#define GRAPHICS_ROW_MAX 144
#define GRAPHICS_LINE_MAX 160

//LCDC bits
#define LCDC_LCD_ENABLE 128
#define LCDC_WINDOW_TILE_MAP 64
#define LCDC_WINDOW_ENABLE 32
#define LCDC_BG_WINDOW_TILE_DATA 16
#define LCDC_BG_TILE_MAP 8
#define LCDC_OBJ_SIZE 4
#define LCDC_OBJ_ENABLE 2
#define LCDC_BG_WINDOW_PRIORITY 1

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

struct screenData_t
{
	unsigned char rowNumber;
	unsigned char lineNumber;
	unsigned char interruptLine;

	unsigned char scrollX;
	unsigned char scrollY;
	unsigned char winodwX;
	unsigned char winodwY;

	unsigned char inactiveBuffer;
	GdkPixbuf* pixbuf[2];
};

//local functions
//get palet from a pixel in a tile
static unsigned char getPalete(tile *tile, unsigned char pixelX, unsigned char pixelY)
{
	unsigned char palete = 0;
	tileLine* line = &tile->line[pixelY];
	palete = (line->lsByte >> (7 - pixelX)) & 1;
	palete |= ((line->msByte >> (7 - pixelX)) & 1) << 1;
	return palete;
}

static void putPixel(GdkPixbuf* buffer, unsigned char x, unsigned char y, unsigned char color, unsigned char pallet)
{
	//calculate the color, just dmg 
	//i'm currently not using pallet
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

screenData* screenData_init()
{
	screenData* data = malloc(sizeof(screenData));
	memset(data, 0, sizeof(screenData));

	data->pixbuf[0] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_LINE_MAX, GRAPHICS_ROW_MAX);
	data->pixbuf[1] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, GRAPHICS_LINE_MAX, GRAPHICS_ROW_MAX);

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

void perform_LCD_operation(screenData* screenData, RAM* RAM_ptr, framebuffer* fb)
{
	//if the lcd is disabled, just return
	if(!(RAM_ptr[RAM_LOCATION_LCDC] & LCDC_LCD_ENABLE))
	{
		//should also technically black out the screen
		//TODO: do that sometime
		return;
	}

	//testing code
	//display the first couple of tiles to the screen
	unsigned short bgTileLocation = 0;
	if(RAM_ptr[RAM_LOCATION_LCDC] & LCDC_BG_WINDOW_TILE_DATA)
		bgTileLocation = 0x8800;
	else
		bgTileLocation = 0x8000;

	int tileId = 0x30;
	tile* currentTile = (tile*)(RAM_ptr + (bgTileLocation + (tileId * 16)));

	for(unsigned char y = 0; y < 8; y++)
	{
		for(unsigned char x = 0; x < 8; x++)
		{
			unsigned char pixpallete = getPalete(currentTile, x, y);
			putPixel(screenData->pixbuf[0], x, y, pixpallete, 0);
		}
	}
	
	framebuffer_data* fb_data = get_framebuffer_data(fb);
	fb_data->framebuff = screenData->pixbuf[0];
	fb_data->buffer_id = fb_data->buffer_id ? 0 : 1;
	unlock_shared_data(fb);
}