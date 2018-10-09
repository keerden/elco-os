#include <kstring.h>

/*** kstrchr ***
 * The strchr function locates the first occurrence of c (converted to a char) in the
 * string pointed to by s. The terminating null character is considered to be part of the
 * string.
 * 
 * Returns
 * 
 * The kstrchr function returns a pointer to the located character, or a null pointer if the
 * character does not occur in the string.
****/

char *kstrchr(const char *s, int c)
{
    unsigned char v = (unsigned char) c;

    while(*s != '\0' && *s != v){
        s++;
    }


	return (*s != '\0' || v == '\0') ? (char *) s : NULL;
}


