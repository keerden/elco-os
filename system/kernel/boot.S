#define ASM_FILE        1
#include <elco-os/config.h>
#include "config.h"
#include "const.h"  
#include <elco-os/multiboot.h>

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




.section .bss, "aw", @nobits
	.align 4096
.global boot_page_directory
boot_page_directory:
	.skip 4096
.global boot_page_table1
boot_page_table1:
	.skip 4096
.global pstack_page_table
pstack_page_table:
	.skip 4096
# Further page tables may be required if the kernel grows beyond 3 MiB.

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	# Physical address of boot_page_table1.
	movl $(kvirt_to_phys(boot_page_table1)), %edi
	
	movl $0, %esi		# First address to map is address 0.
	movl $1024, %ecx	# 1024 pages to map.

loop1:
	# map everything before the location of the kernel.
	cmpl $(kvirt_to_phys(_end_of_kernel)), %esi
	jge set_PDE

	# Map physical address as "present (bit0), writable (bit1)". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	# TODO: permissions for pages of different kernel sections
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)
	addl $4096, %esi	# step 4kb.
	addl $4, %edi		# next entry.
	loop loop1
set_PDE:

	# idmap everything before virtual kernel space, using 4mb pages
	movl $(kvirt_to_phys(boot_page_directory)), %edi
	movl $0, %esi		# First address to map is address 0.
	movl $(KERNEL_PD_NUMBER), %ecx	

idmap:
	movl %esi, %edx
	orl $0x00000083, %edx	# present (bit0), writable (bit1), 4mb (bit7)
	movl %edx, (%edi)
	addl $0x400000, %esi	# add 4Mb.
	addl $4, %edi			# next entry.
	loop idmap

	movl $(kvirt_to_phys(boot_page_table1) + 0x003), (%edi)	# map kernel ptable, set present (bit0), writable (bit1)
	# map pstack ptable, set present (bit0), writable (bit1)
	movl $(kvirt_to_phys(pstack_page_table) + 0x003), (kvirt_to_phys(boot_page_directory) + 1022 * 4)	
	# recursive PDE, set present (bit0), writable (bit1)
	movl $(kvirt_to_phys(boot_page_directory) + 0x003), (kvirt_to_phys(boot_page_directory) + 1023 * 4)


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


# C declaration:
#   void switch_to_task(thread_control_block *next_thread)#
#
# WARNING: Caller is expected to disable IRQs before calling, and enable IRQs again after function returns

.global switch_to_task
.type switch_to_task, @function
switch_to_task:
 
    # Save previous task's state
 
    # Notes:
    #   For cdecl; EAX, ECX, and EDX are already saved by the caller and don't need to be saved again
    #   EIP is already saved on the stack by the caller's "CALL" instruction
    #   The task isn't able to change CR3 so it doesn't need to be saved
    #   Segment registers are constants (while running kernel code) so they don't need to be saved
 
    push %ebx
    push %esi
    push %edi
    push %ebp
 
    mov current_proc, %edi    # edi = address of the previous task's "thread control block"
    mov %esp, (%edi)        	 # Save ESP for previous task's kernel stack in the thread's TCB
 
    # Load next task's state
 
    mov (4*(4+1))(%esp), %esi         # esi = address of the next task's "thread control block" (parameter passed on stack)
    mov %esi,  current_proc   	# Current task's TCB is the next task TCB
 
	
    mov (%esi), %esp        		  # Load ESP for next task's kernel stack from the thread's TCB
    mov 4(%esi), %eax         	  # eax = address of page directory for next task
 #    mov ebx,[esi+TCB.ESP0]        # ebx = address for the top of the next task's kernel stack
  #   mov [TSS.ESP0],ebx            # Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    mov %cr3, %ecx                   # ecx = previous task's virtual address space
 
    cmp %ecx, %eax                   # Does the virtual address space need to being changed?
    je .doneVAS                   #  no, virtual address space is the same, so don't reload it and cause TLB flushes
    mov %eax, %cr3                   #  yes, load the next task's virtual address space
.doneVAS:
 
    pop %ebp
    pop %edi
    pop %esi
    pop %ebx
 
    ret                           # Load next task's EIP from its kernel stack


# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip KERNEL_STACK_SIZE 
stack_top:



