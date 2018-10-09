#include <kstring.h>

/*** kstrncmp ***
 * 
 * The kstrncmp function compares not more than n characters (characters that follow a
 * null character are not compared) from the array pointed to by s1 to the array pointed to
 * by s2.
 * 
 * Returns
 * 
 * The kstrncmp function returns an integer greater than, equal to, or less than zero,
 * accordingly as the possibly null-terminated array pointed to by s1 is greater than, equal
 * to, or less than the possibly null-terminated array pointed to by s2.
 * 
 ***/


int kstrncmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char* a = (const unsigned char*) s1;
	const unsigned char* b = (const unsigned char*) s2;

	while(n && *a != '\0' && *a == *b){
		a++;
		b++;
		n--;
	}
 
	return (n == 0) ? 0 : (*a - *b);
}
