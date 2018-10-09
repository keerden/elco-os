#include <kstring.h>

/*** kstrrchr ***
 * The kstrrchr function locates the last occurrence of c (converted to a char) in the
 * string pointed to by s. The terminating null character is considered to be part of the
 * string..
 * 
 * Returns
 * 
 * The kstrrchr function returns a pointer to the character, or a null pointer if c does not
 * occur in the string.
****/

char *kstrrchr(const char *s, int c)
{
    unsigned char v = (unsigned char) c;
    char* result = NULL;

    for(; *s != '\0';s++){
        if(*s == v)
            result = (char*)  s;
    }
        

	return (v == '\0')? ((char*)  s) : result;
}


