#define ASM_FILE        1
#include <elco-os/config.h>
#include <elco-os/kernel/multiboot.h>

#include "config.h"
#include "const.h"  

# Declare constants used for creating a multiboot header.
.set MULTIBOOT_HEADER_FLAGS,  (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
# checksum of above, to prove we are multiboot
.set CHECKSUM, -(MULTIBOOT_HEADER_FLAGS + MULTIBOOT_HEADER_MAGIC) 

# Declare a header as in the Multiboot Standard.
.section .multiboot

.align 4
.long MULTIBOOT_HEADER_MAGIC
.long MULTIBOOT_HEADER_FLAGS
.long CHECKSUM

.section .bss
.align 0x1000
.global boot_page_directory
boot_page_directory:
	# TODO: remove 'magic' numbers
    # This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
    # All bits are clear except the following:
    # bit 7: PS The kernel page is 4MB.
    # bit 1: RW The kernel page is read/write.
    # bit 0: P  The kernel page is present.
    # This entry must be here -- otherwise the kernel will crash immediately after paging is
    # enabled because it can't fetch the next instruction! It's ok to unmap this page later.
	.set                address,0
    # .int 0x00000083
    # .rept (KERNEL_PD_NUMBER - 1) # unmapped Pages before kernel space.
	#	.int 0 
	 .rept (KERNEL_PD_NUMBER) # ID mapped Pages before kernel space.
		.int (address + 0x00000083) 
		.set address, address + 4096 * 1024
	.endr

    # This page directory entry defines a 4MB page containing the kernel.
    .int 0x00000083
	.rept (1024 - KERNEL_PD_NUMBER - 1) # unmapped Pages after kernel space.
		.int 0 
	.endr
 
# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	# TODO: permissions for pages of different kernel sections
	# Set cr3 to the address of the boot_page_directory.
	movl $(kvirt_to_phys(boot_page_directory)), %ecx
	movl %ecx, %cr3
	
	# Set PSE bit in CR4 to enable 4MB pages.
    movl %cr4, %ecx
    orl $0x00000010, %ecx                          
    movl %ecx, %cr4

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump. 
	lea higher_half, %ecx
	jmp *%ecx


	higher_half:
	movl $stack_top, %esp

	# reset EFLAGS
	pushl   $0
    popf

	# Push the pointer to the Multiboot information structure.
	pushl   %ebx
	# Push the magic value.
	pushl   %eax

	# Initialize the core kernel before running the global constructors.
	call kernel_early

	# Call the global constructors.
#	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
.Lhang:
	hlt
	jmp .Lhang
.size _start, . - _start





# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip KERNEL_STACK_SIZE 
stack_top:



