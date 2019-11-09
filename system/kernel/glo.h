#ifndef _KERNEL_GLO_H
#define _KERNEL_GLO_H

#include "type.h"


extern struct kinfo kinfo;

extern pointer_t boot_page_directory;
extern pointer_t kernel_stack_top;
extern pointer_t kernel_stack_bottom;

/* labels defined by linker */

extern pointer_t _start_of_kernel;
extern pointer_t _end_of_kernel;
extern pointer_t _start_of_kernel_phys;
extern pointer_t _end_of_kernel_phys;


#endif