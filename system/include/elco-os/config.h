#ifndef __ELCO_OS_CONFIG_H
#define __ELCO_OS_CONFIG_H

#define VERSION_MAJOR "0"
#define VERSION_MINOR "2"

#define KERNEL_VIRTUAL_BASE 0xC0000000 //virtual address of kernel space

#define PAGE_SIZE 4096u

#define MAX_BOOT_MMAP_SIZE 128
#define MAX_MMAP_SIZE (MAX_BOOT_MMAP_SIZE + 2) /* max mmap size from bootloader 
                                                + 2 entries for kernel and 
                                                module area's */

#endif
