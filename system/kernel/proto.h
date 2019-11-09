#ifndef _PROTO_H
#define _PROTO_H

#include <kstddef.h>
#include <kstdbool.h>
#include <kstdio.h>

/* memory.c */

int memory_init(void);
int memory_allocate_page(vir_bytes vaddr, int usermode, int writable);
int memory_free_page(vir_bytes vaddr);
int memory_map_addr(vir_bytes vaddr, phys_bytes frame, int usermode, int writable);
int memory_unmap_addr(vir_bytes vaddr);


/* terminal.c */

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_scroll(bool force);
void terminal_print_logo(void);

#ifndef _KERNEL_DEBUG
#define kdebug(...)
#else
#define kdebug(...) kprintf(__VA_ARGS__)
#endif

void kerror(const char* error);
void kpanic(const char* message);


#endif