#include <kstring.h>

/*** kmstrncpy ***
 * 
 * The kstrncpy function copies not more than n characters (characters that follow a null
 * character are not copied) from the array pointed to by s2 to the array pointed to by
 * s1. If copying takes place between objects that overlap, the behavior is undefined.
 *
 * If the array pointed to by s2 is a string that is shorter than n characters, null characters
 * are appended to the copy in the array pointed to by s1, until n characters in all have been
 * written.
 * 
 * Returns
 * 
 * The kstrncpy function returns the value of s1
 * 
 ***/

char *kstrncpy(char *__restrict s1, const char *__restrict s2, size_t n)
{

    char *d = (char *)s1;
    const char *s = (const char *)s2;

    while ((n > 0) && ((*d++ = *s++) != '\0'))
        n--;

    while(n-- > 1)
        *d++ = '\0';

    return s1;
}
