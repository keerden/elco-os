#ifndef __testsuite_h
#define __testsuite_h
#include "kstdlib/malloc_free/kmalloc.h"




typedef enum {USED, FREE, TC, DV} test_chunktype;

struct test_chunkinfo_struct {
    test_chunktype type;
    size_t size;
    void* payload;
};

typedef struct test_chunkinfo_struct test_chunkinfo;

extern int check_heap_integrity(void *heap, size_t heapsize);
extern int check_heap_layout(   test_chunkinfo *expected, 
                                int testsize, 
                                struct kmalloc_state kmstate,
                                void *heap, 
                                size_t heapsize
                            );
extern int check_intree(ktchunk_ptr chunk, ktchunk_ptr root); 
extern int check_bins(void *heap, size_t heapsize, struct kmalloc_state state);
void debugDump(char *desc, void *addr, int len) ;
#endif // !__testsuite_h