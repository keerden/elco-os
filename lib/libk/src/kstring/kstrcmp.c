#include <kstring.h>

/*** kstrcmp ***
 * 
 * The kstrcmp function compares the string pointed to by s1 to the string pointed to by
 * s2.
 * 
 * Returns
 * 
 * The kstrcmp function returns an integer greater than, equal to, or less than zero,
 * accordingly as the string pointed to by s1 is greater than, equal to, or less than the string
 * pointed to by s2.
 * 
 ***/


int kstrcmp(const char *s1, const char *s2)
{
	const unsigned char* a = (const unsigned char*) s1;
	const unsigned char* b = (const unsigned char*) s2;

	while(*a != '\0' && *a == *b){
		a++;
		b++;
	}
		
	return *a - *b;
}
