#include <kstdio.h>
#include "../callback.h"

int kputchar(int ic)
{
	__PUTC_CALLBACK((char) ic);
	return ic;
}
