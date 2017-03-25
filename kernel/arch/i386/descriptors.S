.section .text
.global gdt_flush
.type gdt_flush, @function
.extern gdt 
gdt_flush:
    lgdt gdt_p        # Load the GDT with our '_gdt' which is a special pointer
    mov %ax, 0x10      # 0x10 is the offset in the GDT to our data segment
    mov %ds, %ax
    mov %es, %ax
    mov %fs, %ax
    mov %gs, %ax
    mov %ss, %ax
    jmp $0x08, $flush2   # 0x08 is the offset to our code segment: Far jump!
flush2:
    ret               # Returns back to the C code!
