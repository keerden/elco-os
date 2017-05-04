
#include <kernel/i386.h>
#include <kernel/i386/descriptors.h>
#include <kernel/i386/interrupts.h>
#include <kernel/i386/pic.h>
#include <kernel/i386/timer.h>
#include <kernel/tty.h>

void i386_init(void) {
	terminal_initialize();
    intr_init();
	descriptors_initialize();
	pic_init();
	init_timer(1000);
	intr_enable();


}