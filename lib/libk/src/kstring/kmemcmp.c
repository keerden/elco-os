#include <kstring.h>

/*** kmemcmp ***
 * 
 * The memcmp function compares the first n characters of the object pointed to by s1 to
 * the first n characters of the object pointed to by s2. 
 * 
 * Returns
 * 
 * The kmemcmp function returns an integer greater than, equal to, or less than zero,
 * accordingly as the object pointed to by s1 is greater than, equal to, or less than the object
 * pointed to by s2.
 * 
 ***/


int kmemcmp(const void* s1, const void* s2, size_t n)
{
	const unsigned char* a = (const unsigned char*) s1;
	const unsigned char* b = (const unsigned char*) s2;
	int r;

	while(n--) {
		r = *a++ - *b++;
		if(r)
			return r;
	}
	return 0;
}
