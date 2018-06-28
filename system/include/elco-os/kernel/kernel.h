#ifndef _KERNEL_H
#define _KERNEL_H

#include <elco-os/kernel/tty.h>
#include <stdio.h>


#ifndef _KERNEL_DEBUG
#define kdebug(...)
#else
#define kdebug(...) printf(__VA_ARGS__)
#endif

void kerror(const char* error);


#endif