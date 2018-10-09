#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <kstddef.h>
#include <kstdbool.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_scroll(bool force);
void terminal_print_logo(void);




#endif
