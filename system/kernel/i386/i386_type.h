/********************************
 * This header files defines some structures
 * specific to the i386 architecture
 * 
 ********************************/

#ifndef _I386_TYPE_H
#define _I386_TYPE_H

#include <kstdint.h>


struct gdt_entry {
    uint16_t    limit_low;
    uint16_t    base_low;
    uint8_t     base_mid;
    uint8_t     access;
    uint8_t     limit_hight:4;
    uint8_t     flags:4;
    uint8_t     base_high;
} __attribute__((packed));

struct gdt_pointer {
    uint16_t    size;
    uint32_t    offset;
} __attribute__((packed));

struct idt_entry {
    uint16_t    offset_low;
    uint16_t    selector;
    uint8_t     zero;
    uint8_t     type_atr;
    uint16_t    offset_high;
    
} __attribute__((packed));

struct idt_pointer {
    uint16_t    limit;
    uint32_t    base;
} __attribute__((packed));

typedef struct intr_stack
{
    uint32_t gs, fs, es, ds;                          // manually pushed segments
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by 'pusha' 
    uint32_t int_no, err_code;                        // pushed error and interrupt number
    uint32_t eip, cs, eflags, useresp, ss;            // pushed by the processor automatically  
} intr_stack_t; 

#endif