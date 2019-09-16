#include <kstdio.h>
#include <kstdlib.h>
#include "../callback.h"

__attribute__((__noreturn__))
void kabort(void)
{
	// TODO: Add proper kernel panic.
	kprintf("Kernel Panic: kabort()\n");
	__ABORT_CALLBACK();
	while ( 1 ) { }
	__builtin_unreachable();
}
