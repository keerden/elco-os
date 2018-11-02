#include <kstddef.h>
#include <kstdint.h>
#include <kstring.h>
#include <kstdio.h>
#include <kstdlib.h>

#include <elco-os/kernel/kernel.h>
#include <elco-os/kernel/tty.h>
#include <elco-os/kernel/multiboot.h>
#include "physmem.h"
#include "i386/i386.h"
#include "i386/interrupts.h"

extern pointer_t _start_of_kernel;
extern pointer_t _end_of_kernel;
extern pointer_t _start_of_kernel_phys;
extern pointer_t _end_of_kernel_phys;

void kernel_early(uint32_t magic, multiboot_info_t *bootinfo)
{
	i386_init();
	if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
		kerror("Elco-OS should be loaded with a valid multiboot bootloader!\n");
		kabort();
	}
	
	kdebug("start virt: %p phys: %p end virt: %p phys: %p \n", _start_of_kernel, _start_of_kernel_phys, _end_of_kernel, _end_of_kernel_phys);

	physmem_init(bootinfo, (physical_addr) _start_of_kernel, (physical_addr) _end_of_kernel);
	
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