#include <stddef.h>
#include <stdint.h>
#include <i386/io.h>
#include <i386/interrupts.h>
#include <i386/timer.h>
#include <kernel.h>

uint32_t tick = 0;
uint32_t tick2 = 0;

static void timer_callback(intr_stack_t *r)
{

    tick2++;
    if(tick2 % 1000 == 0){
        tick++;
        kdebug("1 second passed\n");

    }

}

void init_timer(uint32_t frequency)
{
   // Firstly, register our timer callback.
   irq_set_handler(0, &timer_callback);

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
} 