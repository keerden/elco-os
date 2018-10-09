#include "unity.h"
#include "testsuite.h"
#include <string.h>
#include <stdio.h>

#include <kstdlib.h>
#include <libk.h>
#include "kstdlib/malloc_free/kmalloc.h"
#include "kstdlib/malloc_free/kmalloc_util.h"


#define HEAPSIZE 4096

uint8_t heap[HEAPSIZE] __attribute__((aligned(8)));

void setUp(void)
{
    memset(heap, 0xAA, HEAPSIZE);
    libk_init_heap((void *)heap, HEAPSIZE);
}

void test_MallocLargeSingle(void) //from top
{
    struct kmalloc_state state;
    void *mem;

    mem = kmalloc(567);
    memset(mem, '1', 567);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected[2] = {
        {USED, 568 + 8, mem},
        {TC, HEAPSIZE -DUMMYSIZE - (568 + 8), NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}


void test_MultipleLargeMalloc(void)
{
    void *mem1, *mem2, *mem3, *mem4;
    struct kmalloc_state state;

    mem1 = kmalloc(568);
    mem2 = kmalloc(256);
    mem3 = kmalloc(512);
    mem4 = kmalloc(400);
    memset(mem1, '1',568);
    memset(mem2, '2',256);
    memset(mem3, '3',512);
    memset(mem4, '4',400);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[5] =    {
                                    {USED, 568 + 8, mem1},
                                    {USED, 256 + 8, mem2},
                                    {USED, 512 + 8, mem3},
                                    {USED, 400 + 8, mem4},
                                    {TC, HEAPSIZE - DUMMYSIZE - 4 * 8 - 1736, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 5, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));

}


void test_MallocLargeFromBin(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(0x380);
    mem2 = kmalloc(0x100);
    kfree(mem1);
    mem3 = kmalloc(0x380);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[3] =   {{USED, 0x380 + 8, mem3},
                                    {USED, 0x100 + 8, mem2},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x490, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocLargeFromLargerBin(void)
{
    void *mem1, *mem2, *mem3;
    struct kmalloc_state state;

    mem1 = kmalloc(0x380);
    mem2 = kmalloc(0x100);
    kfree(mem1);
    mem3 = kmalloc(0x180);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[4] =   {{USED, 0x180 + 8, mem3},
                                    {DV, 0x200, NULL},
                                    {USED, 0x100 + 8, mem2},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x490, NULL}};;
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocLargeFitBinLargerDv(void)
{
    void *mem[6];
    struct kmalloc_state state;

    mem[0] = kmalloc(0x400);
    mem[1] = kmalloc(0x100);
    mem[2] = kmalloc(0x200);
    mem[3] = kmalloc(0x100);
    kfree(mem[0]);
    mem[4] = kmalloc(0x1F0); //a split occurs resulting in a dv of 0x210
    kfree(mem[2]);
    mem[6] = kmalloc(0x200);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[6] =   {{USED, 0x1F0 + 8, mem[4]},
                                    {DV, 0x210, NULL},
                                    {USED, 0x100 + 8, mem[1]},
                                    {USED, 0x200 + 8, mem[6]},
                                    {USED, 0x100 + 8, mem[3]},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x820, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 6, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocSmallFitBinBetterDvExhaust(void)
{
    void *mem[6];
    struct kmalloc_state state;

    mem[0] = kmalloc(0x400);
    mem[1] = kmalloc(0x100);
    mem[2] = kmalloc(0x208); // results in chunk of 0x210
    mem[3] = kmalloc(0x100);
    kfree(mem[0]);
    mem[4] = kmalloc(0x1F8); //a split occurs resulting in a dv of 0x208
    kfree(mem[2]);
    mem[6] = kmalloc(0x200);

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[6] =   {{USED, 0x1F8 + 8, mem[4]},
                                    {USED, 0x200 + 8, mem[6]},
                                    {USED, 0x100 + 8, mem[1]},
                                    {FREE, 0x208 + 8, NULL},
                                    {USED, 0x100 + 8, mem[3]},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x828, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 6, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocSmallFitBinBetterDvSplit(void)
{
    void *mem[6];
    struct kmalloc_state state;

    mem[0] = kmalloc(0x400);
    mem[1] = kmalloc(0x100);
    mem[2] = kmalloc(0x240); // results in chunk of 0x248
    mem[3] = kmalloc(0x100);
    kfree(mem[0]);
    mem[4] = kmalloc(0x1C0); //a split occurs resulting in a dv of 0x240
    kfree(mem[2]);           //a chunk of 0x248 is binned
    mem[6] = kmalloc(0x200); //dv is split resulting in a dv of 0x38 = 56

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[7] =   {{USED, 0x1C0 + 8, mem[4]},
                                    {USED, 0x200 + 8, mem[6]},
                                    {DV, 0x38, NULL},
                                    {USED, 0x100 + 8, mem[1]},
                                    {FREE, 0x240 + 8, NULL},
                                    {USED, 0x100 + 8, mem[3]},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x860, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 7, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocLarge_NextBin_DV_Better(void)
{
    void *mem[6];
    struct kmalloc_state state;

    mem[0] = kmalloc(0x300);
    mem[1] = kmalloc(0x100);
    mem[2] = kmalloc(0x380); // results in chunk of 0x388
    mem[3] = kmalloc(0x100);
    kfree(mem[0]);
    mem[4] = kmalloc(0xF0); //a split occurs resulting in a dv of 0x210
    kfree(mem[2]);           //a chunk of 0x388 is binned
    mem[6] = kmalloc(0x178); //dv is split resulting in a dv of 0x90 

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[7] =   {{USED, 0xF0 + 8, mem[4]},
                                    {USED, 0x178 + 8, mem[6]},
                                    {DV, 0x90, NULL},
                                    {USED, 0x100 + 8, mem[1]},
                                    {FREE, 0x380 + 8, NULL},
                                    {USED, 0x100 + 8, mem[3]},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x8A0, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 7, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocLarge_NextBin_DV_Larger(void)
{
    void *mem[6];
    struct kmalloc_state state;

    mem[0] = kmalloc(0x400);
    mem[1] = kmalloc(0x100);
    mem[2] = kmalloc(0x280); // results in chunk of 0x288
    mem[3] = kmalloc(0x100); // 0:0x408 - 1: 0x108 - 2: 0x288 - 3: 0x108 - top
    kfree(mem[0]);           // FREE:0x408 - 1: 0x108 - 2: 0x288 - 3: 0x108 - top
    mem[4] = kmalloc(0xF0); //a split occurs resulting in a dv of 0x310
                            // 4:0xF8 - DV:0x310 - 1: 0x108 - 2: 0x288 - 3: 0x108 - top
    kfree(mem[2]);          // 4:0xF8 - DV:0x310 - 1: 0x108 - FREE: 0x288 - 3: 0x108 - top
    mem[6] = kmalloc(0x178); // binned schunk is split. dv replaced with 0x108
                            // 4:0xF8 - FREE:0x310 - 1: 0x108 - 6: 0x180 - DV:0x108 - 3: 0x108 - top

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[7] =   {{USED, 0xF0 + 8, mem[4]},
                                    {FREE, 0x310, NULL},
                                    {USED, 0x100 + 8, mem[1]},
                                    {USED, 0x178 + 8, mem[6]},
                                    {DV, 0x108, NULL},
                                    {USED, 0x100 + 8, mem[3]},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x8A0, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 7, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocLarge_NextBin_DV_SameBin_Better(void)
{
   void *mem[6];
    struct kmalloc_state state;

    mem[0] = kmalloc(0x300);
    mem[1] = kmalloc(0x100);
    mem[2] = kmalloc(0x280); // results in chunk of 0x288
    mem[3] = kmalloc(0x100); // 0:0x308 - 1: 0x108 - 2: 0x288 - 3: 0x108 - top
    kfree(mem[0]);           // FREE:0x308 - 1: 0x108 - 2: 0x288 - 3: 0x108 - top
    mem[4] = kmalloc(0xF0); //a split occurs resulting in a dv of 0x210
                            // 4:0xF8 - DV:0x210 - 1: 0x108 - 2: 0x288 - 3: 0x108 - top
    kfree(mem[2]);          // 4:0xF8 - DV:0x210 - 1: 0x108 - FREE: 0x288 - 3: 0x108 - top
    mem[6] = kmalloc(0x178); // dv is split resulting in dv of 0x090
                            // 4:0xF8 - 6:0x180 - DV:0x90 - 1: 0x108 - FREE: 0x288 - 3: 0x108 - top

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[7] =   {{USED, 0xF0 + 8, mem[4]},
                                    {USED, 0x178 + 8, mem[6]},
                                    {DV, 0x90, NULL},
                                    {USED, 0x100 + 8, mem[1]},
                                    {FREE, 0x288, NULL},
                                    {USED, 0x100 + 8, mem[3]},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x7A0, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 7, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}

void test_MallocLarge_NextBin_DV_SameBin_Larger(void)
{
    void *mem[6];
    struct kmalloc_state state;

   mem[0] = kmalloc(0x300);
    mem[1] = kmalloc(0x100);
    mem[2] = kmalloc(0x200); // results in chunk of 0x208
    mem[3] = kmalloc(0x100); // 0:0x308 - 1: 0x108 - 2: 0x208 - 3: 0x108 - top
    kfree(mem[0]);           // FREE:0x308 - 1: 0x108 - 2: 0x208 - 3: 0x108 - top
    mem[4] = kmalloc(0xF0); //a split occurs resulting in a dv of 0x210
                            // 4:0xF8 - DV:0x210 - 1: 0x108 - 2: 0x208 - 3: 0x108 - top
    kfree(mem[2]);          // 4:0xF8 - DV:0x210 - 1: 0x108 - FREE: 0x208 - 3: 0x108 - top
    mem[6] = kmalloc(0x178); // free chunk is split, creating dv of 88. old dv is binned 
                            // 4:0xF8 - FREE:0x210 - 1: 0x108 - 8: 0x180 - DV:0x88 - 3: 0x108 - top

    state = kmalloc_debug_getstate();
    test_chunkinfo expected[7] =   {{USED, 0xF0 + 8, mem[4]},
                                    {FREE, 0x210, NULL},
                                    {USED, 0x100 + 8, mem[1]},
                                    {USED, 0x178 + 8, mem[6]},
                                    {DV, 0x88, NULL},
                                    {USED, 0x100 + 8, mem[3]},
                                    {TC, HEAPSIZE - DUMMYSIZE - 0x720, NULL}};
    TEST_ASSERT_FALSE(check_heap_integrity(heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 7, state, heap, HEAPSIZE));
    TEST_ASSERT_FALSE(check_bins(heap,  HEAPSIZE, state));
}





