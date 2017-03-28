#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel.h>
#include <kernel/tty.h>
#include <i386.h>


#include <i386/interrupts.h>

void kernel_early(void)
{
	i386_init();
}

extern uint32_t tick;
void kernel_main(void)
{
	int a = 0;
 	terminal_print_logo();
	kdebug("testing\n");


	while(1);

}

void kerror(const char* error) {
	terminal_writestring("Kerror: ");
	terminal_writestring(error);
}