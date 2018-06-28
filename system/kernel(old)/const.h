#ifndef _KERNEL_CONST_H
#define _KERNEL_CONST_H

#include <elco-os/config.h>




#define phys_to_kvirt(addr) (addr + KERNEL_VIRTUAL_BASE)
#define kvirt_to_phys(addr) (addr - KERNEL_VIRTUAL_BASE)





#define KERNEL_PD_NUMBER (KERNEL_VIRTUAL_BASE >> 22)

#endif