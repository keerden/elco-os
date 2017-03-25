#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stddef.h>
#include <stdint.h>

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


void gdt_initialize();

#endif