#include <kstring.h>

/*** memset ***
 * The memset function copies the value of c (converted to an unsigned char) into
 * each of the first n characters of the object pointed to by s.
 *
 * Returns:
 * The memset function returns the value of s.
****/



void* kmemset(void *s, int c, size_t n)
{
	unsigned char* p = (unsigned char*) s;
	const unsigned char u = (unsigned char) c;

	while(n--){
		*p++ = u;
	}
	
	return s;
}
