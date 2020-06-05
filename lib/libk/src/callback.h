#ifndef __LIBK_KMALLOC_H
#define __LIBK_KMALLOC_H

#include <kstdint.h>


void *__SBRK_CALLBACK(intptr_t increment);
void __ABORT_CALLBACK(void);
void __PUTC_CALLBACK(char);


#endif