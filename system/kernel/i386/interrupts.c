#include "../kernel.h"
#include "i386.h"

#include <kstddef.h>
#include <kstdint.h>
#include <kstdlib.h>





void exception_handler(uint32_t int_no, uint32_t error);
void irq_handler(uint32_t int_no);

void (*exception_handlers[EXCEPTION_COUNT])(uint32_t error);
void (*irq_handlers[IRQ_COUNT])(void);



void arch_init_interrupts(void) {
    for(int i = 0; i < EXCEPTION_COUNT; i++)
        exception_handlers[i]= NULL;
    for(int i = 0; i < IRQ_COUNT; i++)
        irq_handlers[i]= NULL;
    pic_init();
}


void exception_set_handler(int intr_no, void (*handler)(uint32_t error)) {
    if (intr_no >= EXCEPTION_COUNT)
        return;
    exception_handlers[intr_no] = handler;
}

void exception_clear_handler(int intr_no) {
    if (intr_no >= EXCEPTION_COUNT)
        return;
    exception_handlers[intr_no] = NULL;
}


void exception_handler(uint32_t int_no, uint32_t error)
{
    void (*handler)(uint32_t error);
    handler = exception_handlers[int_no];
    if(handler != NULL){
        handler(error);
    }else{
           kprintf("exception no: %u error: %u\n", int_no, error);
           kpanic("unhandled exception!");
    }
} 

void irq_set_handler(int intr_no, void (*handler)(void)) {
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


void DEBUG(){
    kprintf("DEBUG\n");
    kprintf("DEBUG2\n");
}

void irq_handler(uint32_t int_no) {
    void (*handler)(void);
    handler = irq_handlers[int_no];
    if(handler != NULL){
        handler();
    }else{
            DEBUG();
           kprintf("unhandled INT no: %u\n", int_no);
           kpanic("unhandled IRQ!");

    }
    pic_eoi((uint8_t) int_no);
}





void arch_timer_init(uint32_t frequency, void (*callback)(void))
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

   irq_set_handler(0, callback);

} 