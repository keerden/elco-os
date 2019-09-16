#include "i386.h"

#include <kstddef.h>
#include <kstdint.h>
#include <kstring.h>



void gdt_initialize(void);
void idt_initialize(void);
void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void idt_set_entry(int index, uint32_t offset, uint16_t selector, uint8_t attr);

struct gdt_pointer gdt_p;
struct gdt_entry gdt[GDT_SIZE];
struct idt_pointer idt_p;
struct idt_entry idt[IDT_SIZE];

void arch_init_tables(void) {
    gdt_initialize();
    idt_initialize();
}


void gdt_initialize(void) {
    gdt_p.offset    =   (uint32_t) &gdt;
    gdt_p.size = (sizeof(struct gdt_entry) * GDT_SIZE) - 1;

    gdt_set_entry(GDT_DUMMY_INDEX,0,0,0,0);
    gdt_set_entry(  GDT_RING0_CODE_INDEX,
                    0,
                    0xFFFFF, 
                    GDT_ACC_PRESENT | GDT_ACC_PRIV0 | GDT_ACC_RESV | GDT_ACC_EXEC | GDT_ACC_RW,
                    GDT_FLAG_GRAN4K | GDT_FLAG_32BIT
                );
    gdt_set_entry(  GDT_RING0_DATA_INDEX,
                    0,
                    0xFFFFF, 
                    GDT_ACC_PRESENT | GDT_ACC_PRIV0 | GDT_ACC_RESV | GDT_ACC_RW,
                    GDT_FLAG_GRAN4K | GDT_FLAG_32BIT
                );
    gdt_set_entry(  GDT_RING3_CODE_INDEX,
                    0,
                    0xFFFFF, 
                    GDT_ACC_PRESENT | GDT_ACC_PRIV3 | GDT_ACC_RESV | GDT_ACC_EXEC | GDT_ACC_RW,
                    GDT_FLAG_GRAN4K | GDT_FLAG_32BIT
                );
    gdt_set_entry(  GDT_RING3_DATA_INDEX,
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

    entry.base_low      =   (uint16_t) (base & 0x0000FFFFU);
    entry.base_mid      =   (uint8_t) ((base & 0x00FF0000U) >> 16);
    entry.base_high     =   (uint8_t) ((base & 0xFF000000U) >> 24);
    entry.limit_low     =   (uint16_t) (limit & 0x0FFFFU);
    entry.limit_hight   =   (uint8_t) ((limit & 0xF0000U) >> 16);
    entry.access        =   access;
    entry.flags         =   flags & 0xFU;

    gdt[index] = entry;
}

void idt_initialize(void) {
    idt_p.base    =   (uint32_t) &idt;
    idt_p.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    kmemset(&idt, 0, (sizeof(struct idt_entry) * IDT_SIZE));
    
    idt_set_entry(0, (uint32_t) isr0, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(1, (uint32_t) isr1, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(2, (uint32_t) isr2, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(3, (uint32_t) isr3, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(4, (uint32_t) isr4, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(5, (uint32_t) isr5, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(6, (uint32_t) isr6, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(7, (uint32_t) isr7, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(8, (uint32_t) isr8, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(9, (uint32_t) isr9, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(10, (uint32_t) isr10, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(11, (uint32_t) isr11, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(12, (uint32_t) isr12, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(13, (uint32_t) isr13, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(14, (uint32_t) isr14, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(15, (uint32_t) isr15, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(16, (uint32_t) isr16, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(17, (uint32_t) isr17, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(18, (uint32_t) isr18, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(19, (uint32_t) isr19, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(20, (uint32_t) isr20, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(21, (uint32_t) isr21, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(22, (uint32_t) isr22, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(23, (uint32_t) isr23, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(24, (uint32_t) isr24, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(25, (uint32_t) isr25, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(26, (uint32_t) isr26, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(27, (uint32_t) isr27, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(28, (uint32_t) isr28, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(29, (uint32_t) isr29, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(30, (uint32_t) isr30, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(31, (uint32_t) isr31, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);

    idt_set_entry(32, (uint32_t) irq0, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(33, (uint32_t) irq1, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(34, (uint32_t) irq2, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(35, (uint32_t) irq3, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(36, (uint32_t) irq4, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(37, (uint32_t) irq5, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(38, (uint32_t) irq6, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(39, (uint32_t) irq7, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(40, (uint32_t) irq8, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(41, (uint32_t) irq9, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(42, (uint32_t) irq10, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(43, (uint32_t) irq11, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(44, (uint32_t) irq12, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(45, (uint32_t) irq13, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(46, (uint32_t) irq14, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);
    idt_set_entry(47, (uint32_t) irq15, GDT_RING0_CODE, IDT_ATR_TYPE_32IG | IDT_ATR_PRES);


    idt_flush(&idt_p);            
               
}

void idt_set_entry(int index, uint32_t offset, uint16_t selector, uint8_t attr) {
    struct idt_entry entry;
    if(index >= IDT_SIZE)
        return;

    entry.offset_low    = (uint16_t) (offset & 0x0000FFFFU);
    entry.offset_high   = (uint16_t) ((offset & 0xFFFF0000U) >> 16U);
    entry.selector      = selector;
    entry.type_atr      = attr;
    entry.zero          = 0;
    idt[index] = entry;
}
