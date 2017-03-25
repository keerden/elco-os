#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel.h>

void kernel_early(void)
{
	terminal_initialize();
	gdt_initialize();
}

void kernel_main(void)
{
 	terminal_print_logo();
}

void kerror(const char* error) {
	terminal_writestring("Kerror: ");
	terminal_writestring(error);
}