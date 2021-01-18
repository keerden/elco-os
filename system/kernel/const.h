#ifndef _KERNEL_CONST_H
#define _KERNEL_CONST_H

#include <elco-os/config.h>




#define phys_to_kvirt(addr) (addr + KERNEL_VIRTUAL_BASE)
#define kvirt_to_phys(addr) (addr - KERNEL_VIRTUAL_BASE)

#define KERNEL_PD_NUMBER (KERNEL_VIRTUAL_BASE >> 22)

#define PAGE_ALIGN(vaddr) (((vaddr) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1))
#define PAGE_FLOOR(vaddr) ((vaddr) & ~(PAGE_SIZE - 1))

#define LOWEST_TASK_PRIORITY   16

#endif