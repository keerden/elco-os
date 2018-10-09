#include <kstring.h>

/*** kstrcpy ***
 * 
 * The kstrcpy function copies the string pointed to by s2 (including the terminating null
 * character) into the array pointed to by s1. If copying takes place between objects that
 * overlap, the behavior is undefined.
 * 
 * Returns
 * 
 * The kstrcpy function returns the value of s1.
 * 
 ***/

char *kstrcpy(char *__restrict s1, const char *__restrict s2)
{

    char *d = (char *)s1;
    const char *s = (const char *)s2;

    while (*s != '\0')
        *d++ = *s++;
    *d = '\0';

    return s1;
}
