#ifndef _KERNEL_GLO_H
#define _KERNEL_GLO_H

#include "type.h"


extern struct kinfo kinfo;
extern struct process *current_proc;


extern pointer_t boot_page_directory;

/* labels defined by linker */

extern pointer_t _start_of_kernel;
extern pointer_t _end_of_kernel;
extern pointer_t _start_of_kernel_phys;
extern pointer_t _end_of_kernel_phys;



#endif