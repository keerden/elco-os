/********************************
 * This header files defines interface functions
 * and constants for the arch architecture
 * 
 ********************************/

#ifndef _ARCH_INTERFACE_H
#define _ARCH_INTERFACE_H

#include <kstdint.h>
#include <kstddef.h>

enum arch_display_color
{
	COLOR_BLACK = 0U,
	COLOR_BLUE = 1U,
	COLOR_GREEN = 2U,
	COLOR_CYAN = 3U,
	COLOR_RED = 4U,
	COLOR_MAGENTA = 5U,
	COLOR_BROWN = 6U,
	COLOR_LIGHT_GREY = 7U,
	COLOR_DARK_GREY = 8U,
	COLOR_LIGHT_BLUE = 9U,
	COLOR_LIGHT_GREEN = 10U,
	COLOR_LIGHT_CYAN = 11U,
	COLOR_LIGHT_RED = 12U,
	COLOR_LIGHT_MAGENTA = 13U,
	COLOR_LIGHT_BROWN = 14U,
	COLOR_WHITE = 15U,
};


void arch_init_display(void);
void arch_init_tables(void);
void arch_init_interrupts(void);

void arch_display_putc(char c); 
void arch_display_number(int num, size_t x, size_t y); 
void arch_display_setcolor(enum arch_display_color fg, enum arch_display_color bg); 

void init_timer(uint32_t frequency);

inline void arch_intr_enable(void){
    asm volatile("sti");
}
inline void arch_intr_intr_disable(void){
    asm volatile("cli");
}


#endif