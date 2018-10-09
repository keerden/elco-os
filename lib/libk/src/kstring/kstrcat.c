#include <kstring.h>

/*** kstrcat ***
 * 
 * The strcat function appends a copy of the string pointed to by s2 (including the
 * terminating null character) to the end of the string pointed to by s1. The initial character
 * of s2 overwrites the null character at the end of s1. If copying takes place between
 * objects that overlap, the behavior is undefined.
 * 
 * Returns
 * 
 * The kstrcat function returns the value of s1.
 * 
 ***/

char *kstrcat(char * __restrict s1, const char * __restrict s2)
{

    const char *s = (const char *)s2;
    char *d = s1;

    while(*d != '\0')
        d++;

    while (*s != '\0')
        *d++ = *s++;
    *d = '\0';

    return s1;
}
