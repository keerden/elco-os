#include <kstring.h>

/*** kstrchr ***
 * The kstrpbrk function locates the first occurrence in the string pointed to by s1 of any
 * character from the string pointed to by s2.
 * 
 * Returns
 * 
 * The kstrpbrk function returns a pointer to the character, or a null pointer if no character
 * from s2 occurs in s1.
 * 
****/

char *kstrpbrk(const char *s1, const char *s2)
{
    const char *k;
    while (*s1 != '\0')
    {
        k = s2;
        while (*k != '\0')
        {
            if (*k == *s1)
            {
                return (char*) s1;
            }
            k++;
        }
        s1++;
    }

    return NULL;
}
