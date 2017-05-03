#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <kernel.h>
#include <i386/interrupts.h>
#include <i386/pic.h>



void *exception_handlers[EXCEPTION_COUNT];
void *irq_handlers[IRQ_COUNT];

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
    int int_no = stack->int_no;
    handler = exception_handlers[int_no];
    if(handler != NULL){
        handler(stack);
    }else{
           kerror("unhandled exception!");
           abort();
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
    int int_no = stack->int_no;
    handler = irq_handlers[int_no];
    if(handler != NULL){
        handler(stack);
    }else{
           kerror("unhandled IRQ!");
           abort();
    }
    pic_eoi((uint8_t) int_no);
}