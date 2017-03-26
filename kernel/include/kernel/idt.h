#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stddef.h>
#include <stdint.h>

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


void idt_initialize();

#endif