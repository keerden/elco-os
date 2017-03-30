#define ASM_FILE        1
#include <multiboot.h>

# Declare constants used for creating a multiboot header.
.set MULTIBOOT_HEADER_FLAGS,  (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
# checksum of above, to prove we are multiboot
.set CHECKSUM, -(MULTIBOOT_HEADER_FLAGS + MULTIBOOT_HEADER_MAGIC) 

# Declare a header as in the Multiboot Standard.
.section .multiboot
.global _start_of_kernel
_start_of_kernel:

.align 4
.long MULTIBOOT_HEADER_MAGIC
.long MULTIBOOT_HEADER_FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
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
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
.Lhang:
	hlt
	jmp .Lhang
.size _start, . - _start
.section .kernel_end
.global _end_of_kernel
_end_of_kernel:


