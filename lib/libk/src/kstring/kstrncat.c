#include <kstring.h>

/*** kstrncat ***
 * 
 * The kstrncat function appends not more than n characters (a null character and
 * characters that follow it are not appended) from the array pointed to by s2 to the end of
 * the string pointed to by s1. The initial character of s2 overwrites the null character at the
 * end of s1. A terminating null character is always appended to the result.*  If copying
 * takes place between objects that overlap, the behavior is undefined.
 * 
 * note: The maximum number of characters that can end up in the array pointed to by s1 is
 * strlen(s1)+n+1.
 * 
 * Returns
 * 
 * The kstrncat function returns the value of s1.
 * 
 ***/

char *kstrncat(char * __restrict s1, const char * __restrict s2, size_t n)
{

    const char *s = (const char *)s2;
    char *d = s1;

    while(*d != '\0')
        d++;

    while (*s != '\0' && (n-- > 1))
        *d++ = *s++;
    *d = '\0';

    return s1;
}
