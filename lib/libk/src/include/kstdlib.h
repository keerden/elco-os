#ifndef _STDLIB_H
#define _STDLIB_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <kstddef.h>

__attribute__((__noreturn__))
void kabort(void);

void *kaligned_alloc(size_t alignment, size_t size);
void *kcalloc(size_t nmemb, size_t size);
void kfree(void *ptr);
void *kmalloc(size_t size);
void *krealloc(void *ptr, size_t size);


#ifdef __cplusplus
}
#endif

#endif
