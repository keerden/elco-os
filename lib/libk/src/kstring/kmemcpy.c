#include <kstring.h>

/*** kmemcpy ***
 * 
 * The memcpy function copies n characters from the object pointed to by s2 into the
 * object pointed to by s1. If copying takes place between objects that overlap, the behavior
 * is undefined.
 * 
 * Returns
 *
 * The memcpy function returns the value of s1.
 * 
 ***/


void* kmemcpy(void* restrict s1, const void* restrict s2, size_t n)
{
	
	unsigned char* d = (unsigned char*) s1;
	const unsigned char* s = (const unsigned char*) s2;

	while(n--)
		*d++ = *s++;

	return s1;
}
