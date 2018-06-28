#include <stdio.h>

#if defined(__is_elco_os_kernel)
#include <elco-os/kernel/tty.h>
#endif

int putchar(int ic)
{
#if defined(__is_elco_os_kernel)
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
#else
	// TODO: You need to implement a write system call.
#endif
	return ic;
}
