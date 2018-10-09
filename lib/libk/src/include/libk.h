#ifndef _LIBK_H
#define _LIBK_H 1


#ifdef __cplusplus
extern "C" {
#endif

#include <kstddef.h>
#include <kstdint.h>


void libk_set_callback_sbrk(void* (*callback) (intptr_t));
void libk_set_callback_abort(void (*callback) (void));
void libk_set_callback_putc(void (*callback) (char));

void libk_init_heap(void *heap_addr, size_t heap_size);



#ifdef __cplusplus
}
#endif

#endif