#include "../kernel.h"
#include "i386.h"

#include <kstdbool.h>
#include <kstring.h>




static void display_scroll(bool force);
static void display_putentryat(char c, uint8_t color, size_t x, size_t y);





static inline uint8_t make_color(enum arch_display_color fg, enum arch_display_color bg);
static inline uint16_t make_vgaentry(char c, uint8_t color);

static size_t display_row;
static size_t display_column;
static uint8_t display_color;
static uint16_t* display_buffer;
static bool display_initialized;

void arch_init_display(void)
{
	display_row = 0;
	display_column = 0;
	display_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	display_buffer = (uint16_t*) VGA_MEMORY_VADDR;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			display_buffer[index] = make_vgaentry(' ', display_color);
		}
	}
	display_initialized = true;
}

void arch_display_putc(char c)
{
	size_t x;
	if(!display_initialized)
		return;

	switch (c) {
		case '\n':
			display_column = 0;
			display_row++;
			break;
		case '\r':
			display_column = 0;
			break;
		case '\b':
			if(display_column)
				display_column--;
			
			display_putentryat(' ', display_color, display_column, display_row);
			break;
		case '\t':
			x = display_column >> 2;
			display_column = (x + 1) << 2;
			break;
		default:
			display_putentryat(c, display_color, display_column, display_row);
			display_column++;
			break;
	}

	if (display_column >= VGA_WIDTH )
	{
		display_column -= VGA_WIDTH;
		display_row++;
	}
	display_scroll(false);
}

void arch_display_setcolor(enum arch_display_color fg, enum arch_display_color bg)
{
	display_color = make_color(fg, bg);
}


static void display_scroll(bool force)
{
	//check if we need to scroll
	if(display_row < VGA_HEIGHT )
	{
		if(!force)
			return;
		display_row--;
	}else
		display_row = VGA_HEIGHT - 1; 

	kmemmove(display_buffer, display_buffer + VGA_WIDTH,(VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(*display_buffer));
	
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH; 
		display_buffer[index + x] = make_vgaentry(' ', display_color);
	}

}

static void display_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	display_buffer[index] = make_vgaentry(c, color);
}



static inline uint8_t make_color(enum arch_display_color fg, enum arch_display_color bg)
{
	return (uint8_t) (fg | (bg << 4));
}

static inline uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = (uint16_t) c;
	uint16_t color16 = (uint16_t) color;
	return (uint16_t) (c16 | (color16 << 8));
}



