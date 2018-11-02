#include <kstddef.h>
#include <kstdint.h>
#include <kstdlib.h>
#include <elco-os/kernel/kernel.h>
#include "interrupts.h"
#include "pic.h"



void (*exception_handlers[EXCEPTION_COUNT])(intr_stack_t *r);
void (*irq_handlers[IRQ_COUNT])(intr_stack_t *r);

void intr_init() {
    for(int i = 0; i < EXCEPTION_COUNT; i++)
        exception_handlers[i]= NULL;
    for(int i = 0; i < IRQ_COUNT; i++)
        irq_handlers[i]= NULL;
    pic_init();
}


void exception_set_handler(int intr_no, void (*handler)(intr_stack_t *r)) {
    if (intr_no >= EXCEPTION_COUNT)
        return;
    exception_handlers[intr_no] = handler;
}

void exception_clear_handler(int intr_no) {
    if (intr_no >= EXCEPTION_COUNT)
        return;
    exception_handlers[intr_no] = NULL;
}


void exception_handler(intr_stack_t *stack)
{
    void (*handler)(intr_stack_t *r);
    uint32_t int_no = stack->int_no;
    handler = exception_handlers[int_no];
    if(handler != NULL){
        handler(stack);
    }else{
           kerror("unhandled exception!");
           kabort();
    }
} 

void irq_set_handler(int intr_no, void (*handler)(intr_stack_t *r)) {
    if (intr_no >= IRQ_COUNT)
        return;
    irq_handlers[intr_no] = handler;
    pic_clear_mask(intr_no);
}
void irq_clear_handler(int intr_no) {
    if (intr_no >= IRQ_COUNT)
        return;
    irq_handlers[intr_no] = NULL;
    pic_set_mask(intr_no);
}
void irq_handler(intr_stack_t *stack) {
    void (*handler)(intr_stack_t *r);
    uint32_t int_no = stack->int_no;
    handler = irq_handlers[int_no];
    if(handler != NULL){
        handler(stack);
    }else{
           kerror("unhandled IRQ!");
           kabort();
    }
    pic_eoi((uint8_t) int_no);
}