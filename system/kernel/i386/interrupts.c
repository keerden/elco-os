#include "../kernel.h"
#include "i386.h"

#include <kstddef.h>
#include <kstdint.h>
#include <kstdlib.h>





void exception_handler(intr_stack_t *stack);
void irq_handler(intr_stack_t *stack);

void (*exception_handlers[EXCEPTION_COUNT])(intr_stack_t *r);
void (*irq_handlers[IRQ_COUNT])(intr_stack_t *r);



void arch_init_interrupts(void) {
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
           kprintf("exception no: %u \n");
           kpanic("unhandled exception!");
    }
} 

void irq_set_handler(int intr_no, void (*handler)(intr_stack_t *r)) {
    if (intr_no >= IRQ_COUNT)
        return;
    irq_handlers[intr_no] = handler;
    pic_clear_mask((uint8_t) intr_no);
}
void irq_clear_handler(int intr_no) {
    if (intr_no >= IRQ_COUNT)
        return;
    irq_handlers[intr_no] = NULL;
    pic_set_mask((uint8_t) intr_no);
}

void irq_handler(intr_stack_t *stack) {
    void (*handler)(intr_stack_t *r);
    uint32_t int_no = stack->int_no;
    handler = irq_handlers[int_no];
    if(handler != NULL){
        handler(stack);
    }else{
           kpanic("unhandled IRQ!");
    }
    pic_eoi((uint8_t) int_no);
}




uint32_t tick = 0;


static void timer_callback(intr_stack_t *r)
{
    tick++;
    if(tick > 60){
        kprintf("tick\n");
        tick = 0;
    }

}

void init_timer(uint32_t frequency)
{
   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   uint32_t divisor = 1193180 / frequency;

   // Send the command byte.
   outportb(0x43, 0x36);


   

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );
   // Send the frequency divisor.
   outportb(0x40, l);
   outportb(0x40, h);

    // Firstly, register our timer callback.
   irq_set_handler(0, &timer_callback);

} 