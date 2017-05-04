#ifndef _KERNEL_physmem_H
#define _KERNEL_physmem_H

#include <stdint.h>
#include <kernel/multiboot.h>


//! 8 blocks per byte
#define PHYSMEM_BLOCKS_PER_BYTE 8
 
// block size (4k)
#define PHYSMEM_BLOCK_SIZE	4096
#define PHYSMEM_BLOCK_SIZE_SHIFT  12
#define PHYSMEM_BLOCK_SIZE_MASK  0xFFF




//! physical address
typedef	uint32_t physical_addr;
typedef	const char pointer_t[];


void physmem_init(multiboot_info_t *bootinfo,  physical_addr krnl_strt, physical_addr krnl_end); 
int physmem_set_region (physical_addr base, size_t size);
int physmem_unset_region (physical_addr base, size_t size);
void* kmalloc(void);


#endif