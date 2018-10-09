#ifndef _KSTDIO_H
#define _KSTDIO_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <kstddef.h>
#include <kstdarg.h>


int kprintf(const char* __restrict, ...);
int kputchar(int);
int kputs(const char*);

#ifdef __cplusplus
}
#endif

#endif
