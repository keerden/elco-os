#ifndef _I386_INTERRUPTS_H
#define _I386_INTERRUPTS_H

#include <kstddef.h>
#include <kstdint.h>


#define EXCEPTION_COUNT 32
#define IRQ_COUNT 16


typedef struct intr_stack
{
    uint32_t gs, fs, es, ds;                          // manually pushed segments
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by 'pusha' 
    uint32_t int_no, err_code;                        // pushed error and interrupt number
    uint32_t eip, cs, eflags, useresp, ss;            // pushed by the processor automatically  
} intr_stack_t; 


void intr_init(void);
inline void intr_enable(void){
    asm volatile("sti");
}
inline void intr_disable(void){
    asm volatile("cli");
}

void exception_set_handler(int intr_no, void (*handler)(intr_stack_t *r));
void exception_clear_handler(int intr_no);
void exception_handler(intr_stack_t *stack);

void irq_set_handler(int intr_no, void (*handler)(intr_stack_t *r));
void irq_clear_handler(int intr_no);
void irq_handler(intr_stack_t *stack);

#endif