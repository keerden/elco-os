#include <kstring.h>

/*** memmove ***
 * 
 * The memmove function copies n characters from the object pointed to by s2 into the
 * object pointed to by s1. Copying takes place as if the n characters from the object
 * pointed to by s2 are first copied into a temporary array of n characters that does not
 * overlap the objects pointed to by s1 and s2, and then the n characters from the
 * temporary array are copied into the object pointed to by s1.
 * 
 * Returns
 * The memmove function returns the value of s1.
 ***/

void *kmemmove(void *s1, const void *s2, size_t n)
{
	unsigned char *d = (unsigned char *)s1;
	const unsigned char *s = (const unsigned char *)s2;
	if (s1 < s2)
	{
		while (n--)
			*d++ = *s++;
	}
	else
	{
		d += n - 1;
		s += n - 1;
		while (n--)
			*d-- = *s--;
	}
	return s1;
}
