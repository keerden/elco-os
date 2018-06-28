
#include "i386.h"
#include "i386/descriptors.h"
#include "i386/interrupts.h"
#include "i386/pic.h"
#include "i386/timer.h"
#include <elco-os/kernel/tty.h>

void i386_init(void) {
	terminal_initialize();
    intr_init();
	descriptors_initialize();
	pic_init();
	init_timer(1000);
	intr_enable();


}