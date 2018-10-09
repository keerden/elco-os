#include <kstring.h>

/*** kstrlen
 * 
 * The kstrlen function computes the length of the string pointed to by s.
 * 
 * Returns
 * 
 * The kstrlen function returns the number of characters that precede the terminating null
 * character.
 ***/

size_t kstrlen(const char* s)
{
	const char* p = s;
	while (*p != '\0')
		p++;

	return (size_t) (p - s);
}
