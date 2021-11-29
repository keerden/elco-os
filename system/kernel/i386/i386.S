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


.global tss_flush
.type tss_flush, @function
tss_flush:
    mov $(GDT_TSS), %ax     
    ltr    %ax               
    ret  


/*********** Interrupt macros ***************/

.extern exception_handler
.extern irq_handler

# macro for an ISR that receives an errorcode
.macro ISR_ERRCODE num
.global isr\num
isr\num :
    cli
    call save
    push %eax
    push $\num
    call exception_handler
    jmp resume
.endm


# macro for an ISR that receives no errorcode and therefore pushes a dummy byte
.macro ISR_NOERRCODE num
.global isr\num
isr\num :
    cli
    push $0
    call save
    push %eax
    push $\num
    call exception_handler
    jmp resume
.endm

# macro for an IRQ that is called by the PIC
.macro IRQ num
.global irq\num
irq\num :
    cli
    push $0
    call save
    push $\num
    call irq_handler
    jmp resume
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
# IRQ 7
.global irq7
irq7:
    cli
    # check for spurious interrupt, read ISR register of PIC
    push %eax 
    movw $(PIC_OCW3_ISR_READ), %ax      # sets al to command byte, and clears upper half
    outb %al, $(PIC0_ADDR_C)
    inb $(PIC0_ADDR_C), %al
    bt $7, %ax  # test bit 7, and set carry if set
    jc 0f       # jump if interrupt was set
    
    # spurious interrupt, so pop eax and return
    pop %eax
    iret

    0:
    pop %eax
    push $0
    call save
    push $7
    call irq_handler
    jmp resume


IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

.global do_kernel_call
.global kernel_call
.extern scheduler               # void scheduler(void)
.extern after_context_switch    # void after_context_switch(void)
.extern handle_kernel_call      # void handle_kernel_call(int param)
.extern nested_intr
.extern next_task
.extern current_task

do_kernel_call:     # int do_kernel_call(callnum, arg1, arg2)
    push %ebp
    mov  %esp, %ebp
    mov (8)(%ebp), %eax     # callnum
    mov (12)(%ebp), %ecx    # arg1
    mov (16)(%ebp), %edx    # arg2
    int $128
    pop %ebp
    ret

kernel_call:
    cli
    push %eax   # push eax as 'error code' for our generic interrupt handler
    call save

    call handle_kernel_call # side effect: the eax value in the saved context is set to return value. 
                            # After iret in resume,  eax is loaded with this value.

    jmp resume     

# switch to kernel stack and do stuff
.global save
save:
    pushal                   # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    push %ds                    
    push %es
    push %fs
    push %gs

    mov $(GDT_RING0_DATA), %eax          # load the kernel data segment descriptor
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs

    # save the stack pointer in tasks context
    mov current_task, %eax  # load addresss
    mov %esp, (%eax)        # save pointer

    mov (13*4)(%esp), %eax      # save error code
    mov (12*4)(%esp), %edx      # save return addr

    incl nested_intr
    jnz 0f
    mov $kernel_stack_top, %esp
0:
    jmp *%edx                    # return with error coder in eax

.global resume
resume:

    # check for kernel re-entry
    cmpl $0, nested_intr
    jne 0f

    # run scheduler
    call scheduler
    mov next_task, %esi     # esi = address of the next task's struct = pointer to context
    mov %esi, current_task

    # switch context
    call after_context_switch

    mov (%esi) , %esp       # load context, pointed to by esi 
0:

    decl nested_intr

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popal
    add  $8, %esp     # Cleans up the pushed error code and return addr
    iret              # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP