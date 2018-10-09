#ifndef _KSTRING_H
#define _KSTRING_H 1

#include <kstddef.h>


#ifdef __cplusplus
extern "C" {
#endif

//copying functions

void *kmemcpy(void * __restrict s1, const void * __restrict s2, size_t n);
void *kmemmove(void *s1, const void *s2, size_t n);
char *kstrcpy(char * __restrict s1, const char * __restrict s2);
char *kstrncpy(char * __restrict s1, const char * __restrict s2, size_t n);

//Concatenation functions

char *kstrcat(char * __restrict s1, const char * __restrict s2);
char *kstrncat(char * __restrict s1, const char * __restrict s2, size_t n);

//comparison functions

int kmemcmp(const void *s1, const void *s2, size_t n);
int kstrcmp(const char *s1, const char *s2);
int kstrncmp(const char *s1, const char *s2, size_t n);
/* not implemented
    int strcoll(const char *s1, const char *s2);
    size_t strxfrm(char * __restrict s1, const char * __restrict s2, size_t n);
*/

//search functions
void *kmemchr(const void *s, int c, size_t n);
char *kstrchr(const char *s, int c);
size_t kstrcspn(const char *s1, const char *s2);
char *kstrpbrk(const char *s1, const char *s2);
char *kstrrchr(const char *s, int c);
size_t kstrspn(const char *s1, const char *s2);
char *kstrstr(const char *s1, const char *s2);
char *kstrtok(char * __restrict s1, const char * __restrict s2);

// misc functions
void *kmemset(void *s, int c, size_t n);
size_t kstrlen(const char *s);
// not implemented: char *kstrerror(int errnum);

#ifdef __cplusplus
}
#endif

#endif
