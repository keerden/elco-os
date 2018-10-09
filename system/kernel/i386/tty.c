#include <kstdbool.h>
#include <kstddef.h>
#include <kstdint.h>
#include <kstring.h>
#include <libk.h>
#include <elco-os/kernel/tty.h>

#include "vga.h"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
	libk_set_callback_putc(terminal_putchar);

}

void terminal_scroll(bool force)
{
	//check if we need to scroll
	if(terminal_row < VGA_HEIGHT )
	{
		if(!force)
			return;
		terminal_row--;
	}else
		terminal_row = VGA_HEIGHT - 1; 

	kmemmove(terminal_buffer, terminal_buffer + VGA_WIDTH,(VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(*terminal_buffer));
	
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH; 
		terminal_buffer[index + x] = make_vgaentry(' ', terminal_color);
	}

}



void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
	size_t x;
	switch (c) {
		case '\n':
			terminal_column = 0;
			terminal_row++;
			break;
		case '\r':
			terminal_column = 0;
			break;
		case '\b':
			if(terminal_column)
				terminal_column--;
			
			terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
			break;
		case '\t':
			x = terminal_column >> 2;
			terminal_column = (x + 1) << 2;
			break;
		default:
			terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
			terminal_column++;
			break;
	}

	if (terminal_column >= VGA_WIDTH )
	{
		terminal_column -= VGA_WIDTH;
		terminal_row++;
	}
	terminal_scroll(false);
}

void terminal_write(const char* data, size_t size)
{
	for ( size_t i = 0; i < size; i++ )
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
	terminal_write(data, kstrlen(data));
}


void terminal_print_logo(void)
{
	char logo[800] = 
		"                                                            \n"
		"   #### ##      ####    ####              ####     #####    \n"
		"   ##   ##    ##      ##    ##          ##    ##  ##        \n"
		"   ###  ##    ##      ##    ##  ######  ##    ##   #####    \n"
		"   ##   ##    ##      ##    ##          ##    ##       ##   \n"
		"   #### ####    ####    ####              ####     #####    \n"
		"                                                            \n";
	terminal_setcolor(make_color(COLOR_LIGHT_BROWN, COLOR_BLUE));
	terminal_writestring(logo);
	terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));

}


