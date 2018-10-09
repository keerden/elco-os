#include <kstring.h>

/*** kmemchr ***
 * The kmemchr function locates the first occurrence of c (converted to an unsigned
 * char) in the initial n characters (each interpreted as unsigned char) of the object
 * pointed to by s. The implementation shall behave as if it reads the characters sequentially
 * and stops as soon as a matching character is found.
 * 
 * Returns
 * 
 * The kmemchr function returns a pointer to the located character, or a null pointer if the
 * character does not occur in the object.
****/

void *kmemchr(const void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char*) s;
    const unsigned char f = (unsigned char) c;

    while(n--){
        if(*p == f)
            return p;
        p++;
    }
	return NULL;
}


