#include <kstring.h>

/*** kstrstr ***
 * 
 * The kstrstr function locates the first occurrence in the string pointed to by s1 of the
 * sequence of characters (excluding the terminating null character) in the string pointed to
 * by s2.
 * 
 * Returns
 * 
 * The kstrstr function returns a pointer to the located string, or a null pointer if the string
 * is not found. If s2 points to a string with zero length, the function returns s1.
 ****/

char *kstrstr(const char *s1, const char *s2)
{
    const char *s = s1; 
    
    if(*s2 == '\0')
        return (char*) s1;

    while (*s != '\0')
    {
        const char *p = s;     
        const char *n = s2;   
        while(*s != '\0' && *n != '\0' && *s == *n){
            s++; n++;
        }

        if(*n == '\0') {
            return (char *) p;
        }
        s = p + 1;
    }

    return NULL;
}
