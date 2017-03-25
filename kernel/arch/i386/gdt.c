#include <stddef.h>
#include <stdint.h>

#include <kernel/gdt.h>
#include <kernel/tty.h>

#define GDT_SIZE 5

struct gdt_pointer gdt_p;
struct gdt_entry gdt[GDT_SIZE];

static void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
extern void gdt_flush();

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
    gdt_flush();            
               
}

static void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
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