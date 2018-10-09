#include "unity.h"
#include "testsuite.h"
#include <string.h>
#include <stdio.h>

#include <kstdlib.h>
#include <libk.h>
#include "kstdlib/malloc_free/kmalloc.h"
#include "kstdlib/malloc_free/kmalloc_util.h"


#define HEAPSIZE 4096

uint8_t heap[HEAPSIZE];

void setUp(void)
{
    memset(heap, 0, HEAPSIZE);
    libk_init_heap((void *)heap, HEAPSIZE);
}

void test_LargeFreeSingle(void)
{
    struct kmalloc_state state;

    void *mem;
    mem = kmalloc(1048);
    kfree(mem);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[1] = {
        {TC, HEAPSIZE - DUMMYSIZE, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 1, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_FreeExhaustedTop(void) {
 struct kmalloc_state state;

    void *mem;
    mem = kmalloc(HEAPSIZE - 8);
    kfree(mem);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[1] = {
        {TC, HEAPSIZE - DUMMYSIZE, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 1, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_LargeFreeBetweenUsedChunks(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(1048);
    mem3 = kmalloc(64);

    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[4] = {{USED, 32 + 8, mem1},
                                  {FREE, 1048 + 8, NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 1168, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_LargeFreeUnusedBefore(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(256);
    mem2 = kmalloc(512);
    mem3 = kmalloc(64);

    kfree(mem1);
    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] = {{FREE, 784, NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 856, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}


void test_LargeFreeUnusedAfter(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(1048);
    mem3 = kmalloc(64);

    kfree(mem2);
    kfree(mem1);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] = {{FREE, 1096, NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 1168, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_LargeFreeUnusedUnused(void)
{
    void *mem1, *mem2, *mem3, *mem4;
    struct kmalloc_state state;

    mem1 = kmalloc(64);
    mem2 = kmalloc(1024);
    mem3 = kmalloc(512);
    mem4 = kmalloc(32);

    kfree(mem1);
    kfree(mem3);
    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] = {{FREE, 1624, NULL},
                                  {USED, 32 + 8, mem4},
                                  {TC, HEAPSIZE - DUMMYSIZE - 1664, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}
