.section .text
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
    movl    4(%esp), %eax # Get the pointer to the GDT, passed as a parameter.
    lgdt    (%eax)       # Load the new GDT pointer

    mov $0x10, %ax      # 0x10 is the offset in the GDT to our data segment
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $0x08, $gdt_flush2   # 0x08 is the offset to our code segment: Far jump!
gdt_flush2:
    ret               # Returns back to the C code!

.global idt_flush
.type idt_flush, @function
idt_flush:
    movl    4(%esp), %eax   # Get the pointer to the IDT, passed as a parameter.
    lidt    (%eax)          # Load the new IDT pointer
    ret               