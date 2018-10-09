#include <kstring.h>

/*** kstrchr ***
 * The kstrcspn function computes the length of the maximum initial segment of the string
 * pointed to by s1 which consists entirely of characters not from the string pointed to by
 * s2.
 * 
 * Returns
 * 
 * The kstrcspn function returns the length of the segment.
****/

size_t kstrcspn(const char *s1, const char *s2)
{
    size_t result = 0;
    const char *k;

    while (*s1 != '\0')
    {
        k = s2;
        while (*k != '\0')
        {
            if (*k == *s1)
            {
                return result;
            }
            k++;
        }
        result++;
        s1++;
    }

    return result;
}
