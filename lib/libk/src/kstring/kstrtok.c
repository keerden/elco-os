#include <kstring.h>

/*** kstrtok ***
 * A sequence of calls to the kstrtok function breaks the string pointed to by s1 into a
 * sequence of tokens, each of which is delimited by a character from the string pointed to
 * by s2. The first call in the sequence has a non-null first argument; subsequent calls in the
 * sequence have a null first argument. The separator string pointed to by s2 may be
 * different from call to call.
 * 
 * The first call in the sequence searches the string pointed to by s1 for the first character
 * that is not contained in the current separator string pointed to by s2. If no such character
 * is found, then there are no tokens in the string pointed to by s1 and the strtok function
 * returns a null pointer. If such a character is found, it is the start of the first token.
 * 
 * The strtok function then searches from there for a character that is contained in the
 * current separator string. If no such character is found, the current token extends to the
 * end of the string pointed to by s1, and subsequent searches for a token will return a null
 * pointer. If such a character is found, it is overwritten by a null character, which
 * terminates the current token. The strtok function saves a pointer to the following
 * character, from which the next search for a token will start.
 * 
 * Each subsequent call, with a null pointer as the value of the first argument, starts
 * searching from the saved pointer and behaves as described above.
 * 
 * The strtok function is not required to avoid data races with other calls to the strtok
 * function. The implementation shall behave as if no library function calls the strtok
 * function.
 * 
 * Returns
 * 
 * The kstrtok function returns a pointer to the first character of a token, or a null pointer
 * if there is no token.
 * 
****/

char *kstrtok(char * __restrict s1, const char * __restrict s2)
{
    static char* p = NULL;
    if(s1 != NULL)
        p = s1;
    else if(p == NULL)
        return NULL;

    s1 = p + kstrspn(p, s2);
    p = s1 + kstrcspn(s1, s2);

    if(s1 == p)
        return p = NULL;

    if(*p == '\0'){
        p = NULL;
    } else{
        *p++ = '\0';
    }

    return s1;
}
