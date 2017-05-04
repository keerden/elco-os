#include <stddef.h>
#include <stdint.h>
#include <kernel/i386/pic.h>
#include <kernel/i386/io.h>

#include <kernel/kernel.h>

void pic_init() {
    
    outportb(PIC0_ADDR_C, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    outportb(PIC1_ADDR_C, PIC_ICW1_ICW4 | PIC_ICW1_INIT);

    outportb(PIC0_ADDR_D, PIC0_ICW2);
    outportb(PIC1_ADDR_D, PIC1_ICW2);

    outportb(PIC0_ADDR_D, PIC0_ICW3);
    outportb(PIC1_ADDR_D, PIC1_ICW3);

    outportb(PIC0_ADDR_D, PIC_ICW4_8086);
    outportb(PIC1_ADDR_D, PIC_ICW4_8086);

    outportb(PIC0_ADDR_D, 0xff);
    outportb(PIC1_ADDR_D, 0xff);

}

void pic_eoi(uint8_t irq) {
    if(irq > 7){
        outportb(PIC1_ADDR_C, PIC_EOI);
    }
    outportb(PIC0_ADDR_C, PIC_EOI);
}
void pic_set_mask(uint8_t irq) {
    uint8_t mask;
    if(irq > 15)
        return;

    if(irq < 8) {
        mask = inportb(PIC0_ADDR_D);
        outportb(PIC0_ADDR_D, mask | (1 << irq));
    } else {
         mask = inportb(PIC0_ADDR_D);
        outportb(PIC0_ADDR_D, mask | (1 << (irq - 8)));       
    }
}
void pic_clear_mask(uint8_t irq) {
        uint8_t mask;
    if(irq > 15)
        return;

    if(irq < 8) {
        mask = inportb(PIC0_ADDR_D);
        outportb(PIC0_ADDR_D, mask & ~(1 << irq));
    } else {
         mask = inportb(PIC0_ADDR_D);
        outportb(PIC0_ADDR_D, mask & ~(1 << (irq - 8)));       
    }
}