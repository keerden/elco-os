#ifndef _I386_PIC_H
#define _I386_PIC_H

#include <stddef.h>
#include <stdint.h>

#define IRQ0_VECT 32

#define PIC0_ADDR_C		0x20	/* Master PIC command port */
#define PIC0_ADDR_D		0x21	/* Master PIC data port */	
#define PIC1_ADDR_C		0xA0	/* Slave PIC command port */
#define PIC1_ADDR_D		0xA1	/* Slave PIC data port */

#define PIC_ICW1_ICW4	    0x01		/* ICW4 (not) needed */
#define PIC_ICW1_SINGLE	    0x02		/* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL	    0x08		/* Level triggered (edge) mode */
#define PIC_ICW1_INIT	    0x10		/* Initialization - required! */

#define PIC0_ICW2           IRQ0_VECT       
#define PIC1_ICW2           (IRQ0_VECT + 8)

#define PIC0_ICW3           (0x01 << 2) /* PIC1 is connected to IRQ2 of PIC0 */
#define PIC1_ICW3           (0x02)      /* PIC1 is connected to IRQ2 of PIC0 and therefore has ID 2 */

 
#define PIC_ICW4_8086	    0x01		/* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO	    0x02		/* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define PIC_ICW4_SFNM	    0x10		/* Special fully nested (not) */

#define PIC_EOI		0x20		/* End-of-interrupt command code */



void pic_init(void);
void pic_eoi(uint8_t irq);
void pic_set_mask(uint8_t irq);
void pic_clear_mask(uint8_t irq);

#endif