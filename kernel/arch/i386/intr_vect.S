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


.extern isr_handler

# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
isr_common_stub:
    pusha                   # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push %ds                    
    push %es
    push %fs
    push %gs

    mov $0x10, %ax          # load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax
    push %eax               # Push us the stack

    mov $isr_handler, %eax
    call *%eax               # A special call, preserves the 'eip' register

    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add  $8, %esp     # Cleans up the pushed error code and pushed ISR number
    iret              # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!




