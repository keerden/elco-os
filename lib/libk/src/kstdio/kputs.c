#include <kstdio.h>

int kputs(const char* string)
{
	return kprintf("%s\n", string);
}
