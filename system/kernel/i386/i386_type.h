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

struct tss_entry {
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // everything below here is unusued now.. 
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         
   uint32_t cs;        
   uint32_t ss;        
   uint32_t ds;        
   uint32_t fs;       
   uint32_t gs;         
   uint32_t ldt;      
   uint16_t trap;
   uint16_t iomap_base;    
}  __attribute__((packed));

typedef struct intr_stack
{
    uint32_t gs, fs, es, ds;                          // manually pushed segments
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by 'pusha' 
    uint32_t int_no, err_code;                        // pushed error and interrupt number
    uint32_t eip, cs, eflags, useresp, ss;            // pushed by the processor automatically  
} intr_stack_t; 



#endif