#ifndef _KERNEL_CONST_H
#define _KERNEL_CONST_H

#include <elco-os/config.h>




#define phys_to_kvirt(addr) (addr + KERNEL_VIRTUAL_BASE)
#define kvirt_to_phys(addr) (addr - KERNEL_VIRTUAL_BASE)

#define KERNEL_PD_NUMBER (KERNEL_VIRTUAL_BASE >> 22)

#define PAGE_ALIGN(vaddr) (((vaddr) + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1))
#define PAGE_FLOOR(vaddr) ((vaddr) & ~(PAGE_SIZE - 1))

#define LOWEST_TASK_PRIORITY   16

#define TASK_FLAGS_PREEMPTIBLE  0x0001u
#define TASK_FLAGS_AFTER_SWITCH 0x0002u

#define PID_NO_TASK     INT32_MIN
#define PID_ALL_TASKS   (INT32_MIN + 1)
#define PID_MIN         (INT32_MIN + 2)
#define PID_MAX         INT32_MAX

#define MESSAGE_FLAGS_NONE          0x0000u
#define MESSAGE_FLAGS_SENDING       0x0001u
#define MESSAGE_FLAGS_RECEIVING     0x0002u
#define MESSAGE_FLAGS_SENDREC       0x0003u
#define MESSAGE_FLAGS_DELIVERING    0x0080u


#endif