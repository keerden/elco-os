#include <libk.h>
#include "callback.h"
#include "kstdlib/malloc_free/kmalloc.h"

static void *(*__sbrk_callback)(intptr_t) = NULL;
static void (*__abort_callback)(void) = NULL;
static void (*__putc_callback)(char) = NULL;



void libk_set_callback_sbrk(void* (*callback) (intptr_t)){
    __sbrk_callback = callback;
}

void libk_set_callback_abort(void (*callback) (void)){
    __abort_callback = callback;
}


void libk_set_callback_putc(void (*callback) (char)){
    __putc_callback = callback;
}

void libk_init_heap(void *heap_addr, size_t heap_size) {
    kmalloc_init(heap_addr, heap_size);
}


void *__SBRK_CALLBACK(intptr_t increment)
{
    if (__sbrk_callback != NULL)
        return __sbrk_callback(increment);
    else
        return (void *)-1;
}


void __ABORT_CALLBACK(void)
{
    if (__abort_callback != NULL)
        __abort_callback();
}

void __PUTC_CALLBACK(char c)
{
    if (__putc_callback != NULL)
        __putc_callback(c);
}

