#include "i386_const.h"

.section .text
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
    movl    4(%esp), %eax # Get the pointer to the GDT, passed as a parameter.
    lgdt    (%eax)       # Load the new GDT pointer

    mov $(GDT_RING0_DATA), %ax      
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $(GDT_RING0_CODE), $gdt_flush2   # Far jump!
gdt_flush2:
    ret               # Returns back to the C code!

.global idt_flush
.type idt_flush, @function
idt_flush:
    movl    4(%esp), %eax   # Get the pointer to the IDT, passed as a parameter.
    lidt    (%eax)          # Load the new IDT pointer
    ret               
# macro for an ISR that receives an errorcode
.macro ISR_ERRCODE num
.global isr\num
isr\num :
    cli
    push $\num
    jmp isr_common_stub

.endm

# macro for an ISR that receives no errorcode and therefore pushes a dummy byte
.macro ISR_NOERRCODE num
.global isr\num
isr\num :
    cli
    push $0
    push $\num
    jmp isr_common_stub

.endm

# macro for an IRQ that is called by the PIC
.macro IRQ num
.global irq\num
irq\num :
    cli
    push $0
    push $\num
    jmp irq_common_stub
.endm

# start of the intvect section
.section .intvect

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15



.extern exception_handler

# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
isr_common_stub:
    pusha                   # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push %ds                    
    push %es
    push %fs
    push %gs

    mov $(GDT_RING0_DATA), %ax          # load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax
    push %eax               # Push us the stack

    call exception_handler

    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add  $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret              # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

.extern irq_handler

# This is our common IRQ stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level irq handler,
# and finally restores the stack frame.
irq_common_stub:
    pusha                   # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push %ds                    
    push %es
    push %fs
    push %gs

    mov $(GDT_RING0_DATA), %ax          # load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax
    push %eax               # Push us the stack

    call irq_handler 
  
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add  $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret              # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


