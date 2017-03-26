#ifndef _KERNEL_DESCRIPTORS_H
#define __KERNEL_DESCRIPTORS_H

#include <stddef.h>
#include <stdint.h>

#define GDT_SIZE 5

#define GDT_ACC_PRESENT 0x80
#define GDT_ACC_PRIV0   0x00
#define GDT_ACC_PRIV1   0x20
#define GDT_ACC_PRIV2   0x40
#define GDT_ACC_PRIV3   0x60
#define GDT_ACC_RESV    0x10
#define GDT_ACC_EXEC    0x08
#define GDT_ACC_DC      0x04
#define GDT_ACC_RW      0x02
#define GDT_ACC_AC      0x01

#define GDT_FLAG_GRAN4K 0x8
#define GDT_FLAG_32BIT  0x4

#define IDT_SIZE 5

#define IDT_ATR_TYPE_32TSG      0x05
#define IDT_ATR_TYPE_16IG       0x06
#define IDT_ATR_TYPE_16TRG      0x07
#define IDT_ATR_TYPE_32IG       0x0E
#define IDT_ATR_TYPE_32TRG      0x0F

#define IDT_ATR_S               0x10

#define IDT_ATR_PRIV0           0x00
#define IDT_ATR_PRIV1           0x20
#define IDT_ATR_PRIV2           0x40
#define IDT_ATR_PRIV3           0x60

#define IDT_ATR_PRES            0x80

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


void descriptors_initialize();

void gdt_initialize();
void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
extern void gdt_flush(struct gdt_pointer* ptr);

void idt_initialize();
void idt_set_entry(int index, uint32_t offset, uint16_t selector, uint8_t attr);
extern void idt_flush(struct idt_pointer* ptr);






#endif