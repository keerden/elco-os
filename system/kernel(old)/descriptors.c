#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "i386/descriptors.h"
#include "i386/intr_vect.h"



struct gdt_pointer gdt_p;
struct gdt_entry gdt[GDT_SIZE];
struct idt_pointer idt_p;
struct idt_entry idt[IDT_SIZE];

void descriptors_initialize() {
    gdt_initialize();
    idt_initialize();
}


void gdt_initialize() {
    gdt_p.offset    =   (uint32_t) &gdt;
    gdt_p.size = (sizeof(struct gdt_entry) * GDT_SIZE) - 1;

    gdt_set_entry(0,0,0,0,0);
    gdt_set_entry(  1,
                    0,
                    0xFFFFF, 
                    GDT_ACC_PRESENT | GDT_ACC_PRIV0 | GDT_ACC_RESV | GDT_ACC_EXEC | GDT_ACC_RW,
                    GDT_FLAG_GRAN4K | GDT_FLAG_32BIT
                );
    gdt_set_entry(  2,
                    0,
                    0xFFFFF, 
                    GDT_ACC_PRESENT | GDT_ACC_PRIV0 | GDT_ACC_RESV | GDT_ACC_RW,
                    GDT_FLAG_GRAN4K | GDT_FLAG_32BIT
                );
    gdt_set_entry(  3,
                    0,
                    0xFFFFF, 
                    GDT_ACC_PRESENT | GDT_ACC_PRIV3 | GDT_ACC_RESV | GDT_ACC_EXEC | GDT_ACC_RW,
                    GDT_FLAG_GRAN4K | GDT_FLAG_32BIT
                );
    gdt_set_entry(  4,
                    0,
                    0xFFFFF, 
                    GDT_ACC_PRESENT | GDT_ACC_PRIV3 | GDT_ACC_RESV | GDT_ACC_RW,
                    GDT_FLAG_GRAN4K | GDT_FLAG_32BIT
                );  
    gdt_flush(&gdt_p);            
               
}

void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    struct gdt_entry entry;
    if(index >= GDT_SIZE)
        return;

    entry.base_low      =   base & 0x0000FFFF;
    entry.base_mid      =   (base & 0x00FF0000 ) >> 16;
    entry.base_high     =   (base & 0xFF000000 ) >> 24;
    entry.limit_low     =   limit & 0x0FFFF;
    entry.limit_hight   =   (limit & 0xF0000) >> 16;
    entry.access        =   access;
    entry.flags         =   flags & 0xF;

    gdt[index] = entry;
}

void idt_initialize() {
    idt_p.base    =   (uint32_t) &idt;
    idt_p.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    memset(&idt, 0, (sizeof(struct idt_entry) * IDT_SIZE));
    
    idt_set_entry(0, (uint32_t) isr0, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(1, (uint32_t) isr1, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(2, (uint32_t) isr2, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(3, (uint32_t) isr3, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(4, (uint32_t) isr4, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(5, (uint32_t) isr5, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(6, (uint32_t) isr6, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(7, (uint32_t) isr7, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(8, (uint32_t) isr8, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(9, (uint32_t) isr9, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(10, (uint32_t) isr10, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(11, (uint32_t) isr11, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(12, (uint32_t) isr12, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(13, (uint32_t) isr13, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(14, (uint32_t) isr14, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(15, (uint32_t) isr15, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(16, (uint32_t) isr16, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(17, (uint32_t) isr17, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(18, (uint32_t) isr18, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(19, (uint32_t) isr19, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(20, (uint32_t) isr20, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(21, (uint32_t) isr21, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(22, (uint32_t) isr22, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(23, (uint32_t) isr23, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(24, (uint32_t) isr24, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(25, (uint32_t) isr25, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(26, (uint32_t) isr26, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(27, (uint32_t) isr27, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(28, (uint32_t) isr28, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(29, (uint32_t) isr29, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(30, (uint32_t) isr30, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(31, (uint32_t) isr31, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);

    idt_set_entry(32, (uint32_t) irq0, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(33, (uint32_t) irq1, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(34, (uint32_t) irq2, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(35, (uint32_t) irq3, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(36, (uint32_t) irq4, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(37, (uint32_t) irq5, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(38, (uint32_t) irq6, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(39, (uint32_t) irq7, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(40, (uint32_t) irq8, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(41, (uint32_t) irq9, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(42, (uint32_t) irq10, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(43, (uint32_t) irq11, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(44, (uint32_t) irq12, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(45, (uint32_t) irq13, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(46, (uint32_t) irq14, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(47, (uint32_t) irq15, 0x0008, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);


    idt_flush(&idt_p);            
               
}

void idt_set_entry(int index, uint32_t offset, uint16_t selector, uint8_t attr) {
    struct idt_entry entry;
    if(index >= IDT_SIZE)
        return;

    entry.offset_low    = offset & 0x0000FFFF;
    entry.offset_high   = (offset & 0xFFFF0000) >> 16;
    entry.selector      = selector;
    entry.type_atr      = attr;
    entry.zero          = 0;
    idt[index] = entry;
}