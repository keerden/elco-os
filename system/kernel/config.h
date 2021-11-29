#ifndef _KERNEL_CONFIG_H
#define _KERNEL_CONFIG_H

#include <elco-os/config.h>

#define KERNEL_STACK_SIZE           8192u            //8 Kib stack space
#define KERNEL_INITIAL_HEAP_SIZE    8192u            //initial heap size (page alligned)
#define KERNEL_HEAP_LIMIT           0XFF800000u  

#define KERNEL_PTABLE_ADDR      0xFFC00000u          //virtual address of recursive mapped page tables
#define KERNEL_PDE_ADDR         0xFFFFF000u          //virtual address of the page directory itself
#define KERNEL_PSTACK_TOP       KERNEL_PTABLE_ADDR   //Top of the free pages stack (grows downwards)
#define KERNEL_PSTACK_BOTTOM    0XFF800000u         
#define KERNEL_PSTACK_PTE       0xFFFFE000u          //virtual address of recursive mapped page tables

#define KERNEL_CLOCK_FREQ       1000                    //timer ticks per second
#define TASK_DEFAULT_QUANTUM    10


#endif
