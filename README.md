# elco-os

Custom made kernel for x86.

This project is a toy-project and is not expected to be finished soon..


Currently this kernel is not able to run any user program yet, but the following things are already implemented:

- booting the kernel from any bootloader that follows the multiboot specification (like grub)
- basic memory managment, virtual memory, however swapping pages to/from disk is not supported yet
- multitasking, scheduling and interrupts

Next things to do before user programs could run:
 - implement a kernel/system call system
 - implement usermode tasks
 - implement a fylesystem
 - write some basic drivers, and implement syscalls


elco-os is made with help of lots of information from the osdev wiki:

http://wiki.osdev.org


some interesting  articles:


building a cross-compiler : http://wiki.osdev.org/GCC_Cross-Compiler

custom c library    http://wiki.osdev.org/Creating_a_C_Library
