#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/descriptors.h>
#include <kernel.h>

void kernel_early(void)
{
	terminal_initialize();
	descriptors_initialize();
}

void kernel_main(void)
{
 	terminal_print_logo();
	kdebug("testing\n");
	int a = 2;
	a--;
	kdebug("%d\n", a / (a-1));
}

void kerror(const char* error) {
	terminal_writestring("Kerror: ");
	terminal_writestring(error);
}