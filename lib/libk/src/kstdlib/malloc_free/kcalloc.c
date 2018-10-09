#include <kstdlib.h>
#include <kstring.h>

/*** kcalloc ***
 * 
 * The kcalloc function allocates space for an array of nmemb objects, each of whose size
 * is size. The space is initialized to all bits zero.
 * 
 * Returns
 * 
 * The calloc function returns either a null pointer or a pointer to the allocated space.
 * 
 ***/


void *kcalloc(size_t nmemb, size_t size) {
    size_t msize = size * nmemb;
    void* ptr = kmalloc(msize);
    if(ptr != NULL)
        kmemset(ptr, 0, msize);

    return ptr;
}