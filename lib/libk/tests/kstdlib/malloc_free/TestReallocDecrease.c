#include "unity.h"
#include "testsuite.h"
#include <string.h>
#include <stdio.h>

#include <kstdlib.h>
#include <libk.h>
#include "kstdlib/malloc_free/kmalloc.h"

#define HEAPSIZE 12288

uint8_t heap[HEAPSIZE] __attribute__((aligned(8)));

intptr_t increment;
int cb_ok, cb_fail, cb_abort;

void* callback_ok (intptr_t i){
    cb_ok++;
    increment = i;
    return (void*) NULL;
}

void* callback_fail (intptr_t i){
    cb_fail++;
    increment = i;
    return (void*) -1;
}

void callback_abort (void){
    cb_abort++;
}

void setUp(void)
{
    memset(heap, 0xAA, HEAPSIZE);
    increment = cb_ok = cb_fail = cb_abort = 0;
}

void tearDown(void)
{
}


void test_ReallocNullPtr(void)
{
    struct kmalloc_state state;
    void *mem;
    libk_init_heap((void *)heap, 4096);

    mem = krealloc(NULL, 64 + 4);
    TEST_ASSERT_NOT_NULL(mem);

    memset(mem, '1', 64 + 4);

    state = kmalloc_debug_getstate();
    

    test_chunkinfo expected[2] = {
        {USED, 64 + 8, mem},
        {TC, 4096 - 64 - 8 - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocFreeZeroSize(void)
{
    struct kmalloc_state state;
    void *mem;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(64);
    memset(mem, '1', 64);

    mem = krealloc(mem, 0);
    TEST_ASSERT_NULL(mem);

    state = kmalloc_debug_getstate();
    

    test_chunkinfo expected[1] = {
        {TC, 4096 - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 1, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocSameSize(void)
{
    struct kmalloc_state state;
    void *mem, *mem2;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(64 + 4);
    memset(mem, '1', 64 + 4);

    mem2 = krealloc(mem, 64 + 4);

    TEST_ASSERT_EQUAL_PTR(mem, mem2);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem2, 64 + 4);

    state = kmalloc_debug_getstate();
    

    test_chunkinfo expected[2] = {
        {USED, 64 + 8, mem},
        {TC, 4096 - 64 - 8 - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocUnusedChunk(void)
{
  struct kmalloc_state state;
    void *mem1, *mem2;
    libk_set_callback_abort(callback_abort);
    libk_init_heap((void *)heap, 4096);
    mem1 = kmalloc(64);
    mem2 = kmalloc(64);
    kfree(mem1);

    krealloc(mem1, 32);

    TEST_ASSERT_EQUAL(1, cb_abort);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected[3] = {
        {FREE, 64 + 8, NULL},
        {USED, 64 + 8, mem2},
        {TC, 4096 - DUMMYSIZE - (64 + 8) - (64 + 8), NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocWrongPointer(void)
{
   struct kmalloc_state state;
    uint8_t *mem;
    libk_set_callback_abort(callback_abort);
    libk_init_heap((void *)heap, 4096);
    mem = kmalloc(32);

    krealloc(&mem[1], 64);

    TEST_ASSERT_EQUAL(1, cb_abort);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected[2] = {
        {USED, 32 + 8, mem},
        {TC, 4096 - DUMMYSIZE - (32 + 8), NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}


void test_ReallocDecreaseSmallRemainder(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(64 + 4);
    mem2 = kmalloc(64 + 4);
    memset(mem, '1', 64 + 4);

    mem3 = krealloc(mem, 56 + 4);

    TEST_ASSERT_EQUAL_PTR(mem, mem3);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem3, 56 + 4);

    state = kmalloc_debug_getstate();
    

    test_chunkinfo expected[3] = {
        {USED, 64 + 8, mem3},
        {USED, 64 + 8, mem2},
        {TC, 4096 - 2 * (64 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocDecreaseSplitBeforeTop(void)
{
    struct kmalloc_state state;
    void *mem, *mem2;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128 + 4);
    memset(mem, '1', 64 + 4);

    mem2 = krealloc(mem, 64 + 4);

    TEST_ASSERT_EQUAL_PTR(mem, mem2);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem2, 64 + 4);

    state = kmalloc_debug_getstate();
    
    //debugDump("dump", heap, 12288);

    test_chunkinfo expected[2] = {
        {USED, 64 + 8, mem2},
        {TC, 4096 - (64 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}
void test_ReallocDecreaseSplitBeforeEmptyTop(void)
{
    struct kmalloc_state state;
    void *mem, *mem2;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(4096 - 8 - DUMMYSIZE);
    memset(mem, '1', 64 + 4);
    mem2 = krealloc(mem, 64 + 4);

    TEST_ASSERT_EQUAL_PTR(mem, mem2);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem2, 64 + 4);

    state = kmalloc_debug_getstate();
    
    //debugDump("dump", heap, 12288);

    test_chunkinfo expected[2] = {
        {USED, 64 + 8, mem2},
        {TC, 4096 - (64 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocDecreaseSplitBeforeDV(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128);
    mem2 = kmalloc(128);
    kfree(mem); //create a binned chunk of 128 + 8 size
    mem = kmalloc(64);   //split chunk, so remainder is now dv with size 64

    memset(mem, '1', 32);     
    mem3 = krealloc(mem, 32);   //decrease from 64(+8) to 32 (+8), so dv should become 96

    TEST_ASSERT_EQUAL_PTR(mem, mem3);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem3, 32);

    state = kmalloc_debug_getstate();
    
    //debugDump("dump", heap, 12288);

    test_chunkinfo expected[4] = {
        {USED, 32 + 8, mem3},
        {DV, 96, NULL},
        {USED, 128 + 8, mem2},
        {TC, 4096 - (128 + 8) - 96 - (32 + 8)  - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocDecreaseSplitBeforeBin(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3, *mem4;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128);
    mem2 = kmalloc(128);
    mem3 = kmalloc(128);
    kfree(mem2); //create a binned chunk of 128 + 8 size
    

    memset(mem, '1', 64 + 4);     
    mem4 = krealloc(mem, 64 + 4);   //decrease, split off free chunk and merge

    TEST_ASSERT_EQUAL_PTR(mem, mem4);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem4, 64 + 4);

    state = kmalloc_debug_getstate();
    
    //debugDump("dump", heap, 12288);

    test_chunkinfo expected[4] = {
        {USED, 64 + 8, mem4},
        {FREE, 128 + 8 + 64, NULL},
        {USED, 128 + 8, mem3},
        {TC, 4096 - 3 * (128 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocDecreaseSplitBeforeUsed(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128);
    mem2 = kmalloc(128);
  

    memset(mem, '1', 64 + 4);     
    mem3 = krealloc(mem, 64 + 4);   //decrease, split off free chunk

    TEST_ASSERT_EQUAL_PTR(mem, mem3);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem3, 64 + 4);

    state = kmalloc_debug_getstate();
    
    //debugDump("dump", heap, 12288);

    test_chunkinfo expected[4] = {
        {USED, 64 + 8, mem3},
        {FREE, 64, NULL},
        {USED, 128 + 8, mem2},
        {TC, 4096 - 2 * (128 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}