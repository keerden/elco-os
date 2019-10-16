/********************************
 * This header files defines constants and magic numbers
 * used by i386 specific registers and data structures 
 * 
 ********************************/

#ifndef _I386_CONST_H
#define _I386_CONST_H


/* decriptor table sizes */
#define GDT_SIZE 6      
#define IDT_SIZE 256

/* interrupt count */
#define EXCEPTION_COUNT 32
#define IRQ_COUNT 16


/* GDT descriptors */
#define GDT_DESCR_SIZE          0x08U

#define GDT_DUMMY_INDEX         0
#define GDT_RING0_CODE_INDEX    1
#define GDT_RING0_DATA_INDEX    2
#define GDT_RING3_CODE_INDEX    3
#define GDT_RING3_DATA_INDEX    4
#define GDT_TSS_INDEX           5

#define GDT_DUMMY       (GDT_DUMMY_INDEX * GDT_DESCR_SIZE)
#define GDT_RING0_CODE  (GDT_RING0_CODE_INDEX * GDT_DESCR_SIZE)
#define GDT_RING0_DATA  (GDT_RING0_DATA_INDEX * GDT_DESCR_SIZE)
#define GDT_RING3_CODE  (GDT_RING3_CODE_INDEX * GDT_DESCR_SIZE)
#define GDT_RING3_DATA  (GDT_RING3_DATA_INDEX * GDT_DESCR_SIZE)
#define GDT_TSS         (GDT_TSS_INDEX * GDT_DESCR_SIZE)


/* GDT flags */
#define GDT_ACC_PRESENT 0x80U
#define GDT_ACC_PRIV0   0x00U
#define GDT_ACC_PRIV1   0x20U
#define GDT_ACC_PRIV2   0x40U
#define GDT_ACC_PRIV3   0x60U
#define GDT_ACC_RESV    0x10U
#define GDT_ACC_EXEC    0x08U
#define GDT_ACC_DC      0x04U
#define GDT_ACC_RW      0x02U
#define GDT_ACC_AC      0x01U
#define GDT_ACC_TSS_TYPE 0x09U

#define GDT_FLAG_GRAN4K 0x8U
#define GDT_FLAG_32BIT  0x4U

/* IDT flags */
#define IDT_ATR_TYPE_32TSG      0x05U
#define IDT_ATR_TYPE_16IG       0x06U
#define IDT_ATR_TYPE_16TRG      0x07U
#define IDT_ATR_TYPE_32IG       0x0EU
#define IDT_ATR_TYPE_32TRG      0x0FU

#define IDT_ATR_S               0x10U

#define IDT_ATR_PRIV0           0x00U
#define IDT_ATR_PRIV1           0x20U
#define IDT_ATR_PRIV2           0x40U
#define IDT_ATR_PRIV3           0x60U

#define IDT_ATR_PRES            0x80U

/* display */
#define VGA_WIDTH       80
#define VGA_HEIGHT      25
#define VGA_MEMORY_VADDR 0xC00B8000

/* PIC related constants */

#define IRQ0_VECT 32U

#define PIC0_ADDR_C		0x20U	/* Master PIC command port */
#define PIC0_ADDR_D		0x21U	/* Master PIC data port */	
#define PIC1_ADDR_C		0xA0U	/* Slave PIC command port */
#define PIC1_ADDR_D		0xA1U	/* Slave PIC data port */

#define PIC_ICW1_ICW4	    0x01U		/* ICW4 (not) needed */
#define PIC_ICW1_SINGLE	    0x02U		/* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4	0x04U		/* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL	    0x08U		/* Level triggered (edge) mode */
#define PIC_ICW1_INIT	    0x10U		/* Initialization - required! */

#define PIC0_ICW2           IRQ0_VECT       
#define PIC1_ICW2           (IRQ0_VECT + 8)

#define PIC0_ICW3           (0x01U << 2) /* PIC1 is connected to IRQ2 of PIC0 */
#define PIC1_ICW3           (0x02U)      /* PIC1 is connected to IRQ2 of PIC0 and therefore has ID 2 */

 
#define PIC_ICW4_8086	    0x01U		/* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO	    0x02U		/* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE	0x08U		/* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER	0x0CU		/* Buffered mode/master */
#define PIC_ICW4_SFNM	    0x10U		/* Special fully nested (not) */

#define PIC_EOI		0x20U		/* End-of-interrupt command code */


#endif