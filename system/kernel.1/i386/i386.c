
#include "i386.h"
#include "descriptors.h"
#include "interrupts.h"
#include "pic.h"
#include "timer.h"
#include <elco-os/kernel/tty.h>

void i386_init(void) {
	terminal_initialize();
  //  intr_init();
//	descriptors_initialize();
//	pic_init();
//	init_timer(1000);
//	intr_enable();


}