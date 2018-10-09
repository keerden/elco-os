#include <kstring.h>

/*** kstrchr ***
 * The kstrspn function computes the length of the maximum initial segment of the string
 * pointed to by s1 which consists entirely of characters from the string pointed to by s2.
 * 
 * Returns
 * 
 * The strspn function returns the length of the segment.
****/

size_t kstrspn(const char *s1, const char *s2)
{
    size_t result = 0;
    const char *k;

    while (*s1 != '\0')
    {
        for(k = s2; *k != '\0'; k++){
            if(*k == *s1)
                break;
        }
        if(*k == '\0') { 
            return result;
        }
        result++;
        s1++;
    }

    return result;
}
