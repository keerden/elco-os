#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/kernel.h>
#include <kernel/tty.h>
#include <kernel/i386.h>
#include <kernel/multiboot.h>
#include <kernel/physmem.h>

#include <kernel/i386/interrupts.h>

extern pointer_t _start_of_kernel;
extern pointer_t _end_of_kernel;

void kernel_early(uint32_t magic, multiboot_info_t *bootinfo)
{
	i386_init();
	if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
		kerror("Elco-OS should be loaded with a valid multiboot bootloader!\n");
		abort();
	}
	
	physmem_init(bootinfo, (physical_addr) _start_of_kernel, (physical_addr) _end_of_kernel);
	void* addr0 = kmalloc();
	void* addr1 = kmalloc();
	void* addr2 = kmalloc();

	kfree(addr1);

	void* addr3 = kmalloc();
	void* addr4;
	void* p = 0x123;
	
	while(p != NULL) {

		addr4 = p;
		p = kmalloc();
		
	}

	kdebug("pointers: %p %p %p %p %p\n", addr0, addr1, addr2, addr3, addr4);

	kfree((void*) 0x15A000);
	kfree((void*) 0x16A000);
	addr1 = kmalloc();

	kdebug("free: %p\n", addr1);
}

extern uint32_t tick;
void kernel_main(void)
{
 	terminal_print_logo();
	kdebug("testing\n");

	while(1);

}

void kerror(const char* error) {
	terminal_writestring("Kerror: ");
	terminal_writestring(error);
}