#include "unity.h"
#include "testsuite.h"
#include <string.h>
#include <stdio.h>

#include <kstdlib.h>
#include <libk.h>
#include "kstdlib/malloc_free/kmalloc.h"

#define HEAPSIZE 256

uint8_t heap[HEAPSIZE];

void setUp(void)
{
    memset(heap, 0, HEAPSIZE);
    libk_init_heap((void *)heap, HEAPSIZE);
}

void test_SmallFreeSingle(void)
{
    struct kmalloc_state state;

    void *mem;
    mem = kmalloc(32 + 4);
    kfree(mem);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[1] = {
        {TC, HEAPSIZE - DUMMYSIZE, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 1, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}


void test_SmallFreeBetweenUsedChunks(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(64);

    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[4] = {{USED, 32 + 8, mem1},
                                  {FREE, 32 + 8, NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 152, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeUnusedBefore(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(64);

    kfree(mem1);
    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] = {{FREE, 2 * (32 + 8), NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 152, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeDvBefore(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(64);
    kfree(mem1);
    mem1 = kmalloc(16);
    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[4] = {{USED, 16 + 8, mem1},
                                  {DV, 16 + 32 + 8, NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 152, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeUnusedAfter(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(64);

    kfree(mem2);
    kfree(mem1);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] = {{FREE, 2 * (32 + 8), NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 152, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeDvAfter(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(64);
    kfree(mem2);
    mem2 = kmalloc(16); //mem1 - mem2 - dv - mem3
    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[4] = {{USED, 32 + 8, mem1},
                                  {DV, 32 + 8, NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 152, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeTcAfter(void)
{
    void *mem1, *mem2;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(64);

    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[2] = {{USED, 32 + 8, mem1},
                                  {TC, HEAPSIZE - DUMMYSIZE - 40, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeUnusedUnused(void)
{
    void *mem1, *mem2, *mem3, *mem4;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(32);
    mem4 = kmalloc(32);

    kfree(mem1);
    kfree(mem3);
    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] = {{FREE, 3 * (32 + 8), NULL},
                                  {USED, 32 + 8, mem4},
                                  {TC, HEAPSIZE - DUMMYSIZE - 160, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeUnusedDv(void)
{

    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(64);
    kfree(mem2);
    mem2 = kmalloc(16); //mem1 - mem2 - dv - mem3
    kfree(mem1);        //FREE - mem2 - dv - mem3

    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] = {{DV, 2 * (32 + 8), NULL},
                                  {USED, 64 + 8, mem3},
                                  {TC, HEAPSIZE - DUMMYSIZE - 152, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeUnusedTc(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(64);
    mem3 = kmalloc(32);
    kfree(mem2);

    kfree(mem3);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[2] = {{USED, 32 + 8, mem1},
                                  {TC, HEAPSIZE - DUMMYSIZE - 40, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeDvUnused(void)
{
    void *mem1, *mem2, *mem3, *mem4;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    mem3 = kmalloc(64);
    mem4 = kmalloc(16);
    kfree(mem1);
    mem1 = kmalloc(16); //mem1 - DV - mem2 - mem3 - mem4
    kfree(mem3);        //mem1 - DV - mem2 - FREE - mem4

    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[4] = {{USED, 16 + 8, mem1},
                                  {DV, 16 + (32 + 8) + (64 + 8), NULL},
                                  {USED, 16 + 8, mem4},
                                  {TC, HEAPSIZE - DUMMYSIZE - 176, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_SmallFreeDvTc(void)
{
    void *mem1, *mem2;
    struct kmalloc_state state;

    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    kfree(mem1);
    mem1 = kmalloc(16); //mem1 - dv - mem2 - tc
    kfree(mem2);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[2] = {{USED, 16 + 8, mem1},
                                  {TC, HEAPSIZE - DUMMYSIZE - 24, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state)); 
}

