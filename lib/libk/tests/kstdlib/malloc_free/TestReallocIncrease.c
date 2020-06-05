#include "unity.h"
#include "testsuite.h"
#include <string.h>
#include <stdio.h>

#include <kstdlib.h>
#include <libk.h>
#include "kstdlib/malloc_free/kmalloc.h"

#define HEAPSIZE 12288

uint8_t heap[HEAPSIZE] __attribute__((aligned(8)));
uint8_t testdata[256];

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
    for(int i = 0; i < 256; i++){
        testdata[i] = (uint8_t) i;
    }
}

void tearDown(void)
{
}


void test_ReallocExpandExhaustTop(void)
{
    struct kmalloc_state state;
    void *mem, *mem2;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(64 + 4);
    memset(mem, '1', 64 + 4);

    mem2 = krealloc(mem, 4096 - 8 - DUMMYSIZE);
    TEST_ASSERT_EQUAL_PTR(mem, mem2);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem2, 64 + 4);

    state = kmalloc_debug_getstate();
    

    test_chunkinfo expected[1] = {
        {USED, 4096 - DUMMYSIZE, mem2}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 1, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocExpandExhaustDV(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128);
    mem2 = kmalloc(128);
    kfree(mem); //create a binned chunk of 128 + 8 size
    mem = kmalloc(64 + 4);   //split chunk, so remainder is now dv with size 64

    memset(mem, '1', 64 + 4);     
    mem3 = krealloc(mem, 128 + 4);   //increase from 64(+8) to 128 (+8), so expand and exhaust dv

    TEST_ASSERT_EQUAL_PTR(mem, mem3);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem3, 64 + 4);

    state = kmalloc_debug_getstate();
    
 //   debugDump("dump", heap, 12288);

    test_chunkinfo expected[3] = {
        {USED, 128 + 8, mem3},
        {USED, 128 + 8, mem2},
        {TC, 4096 - (128 + 8) - 96 - (32 + 8)  - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocExpandExhaustBin(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3, *mem4;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128);
    mem2 = kmalloc(128);
    mem3 = kmalloc(128);
    kfree(mem2); //create a binned chunk of 128 + 8 size
    

    memset(mem, '1', 128 + 4);     
    mem4 = krealloc(mem, 256 + 8 + 4);   //increase and use next free chunk

    TEST_ASSERT_EQUAL_PTR(mem, mem4);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem4, 128 + 4);

    state = kmalloc_debug_getstate();
    
    //debugDump("dump", heap, 12288);

    test_chunkinfo expected[3] = {
        {USED, 2 * (128 + 8) , mem4},
        {USED, 128 + 8, mem3},
        {TC, 4096 - 3 * (128 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocExpandSplitTop(void)
{
    struct kmalloc_state state;
    void *mem, *mem2;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(64 + 4);
    memset(mem, '1', 64 + 4);

    mem2 = krealloc(mem, 128 + 4);
    TEST_ASSERT_EQUAL_PTR(mem, mem2);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem2, 64 + 4);

    state = kmalloc_debug_getstate();
    

    test_chunkinfo expected[2] = {
        {USED, 128 + 8, mem2},
        {TC, 4096 - (128 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocExpandSplitDV(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128);
    mem2 = kmalloc(128);
    kfree(mem); //create a binned chunk of 128 + 8 size
    mem = kmalloc(64 + 4);   //split chunk, so remainder is now dv with size 64

    memset(mem, '1', 64 + 4);     
    mem3 = krealloc(mem, 96 + 4);   //increase from 64(+8) to 96 (+8), so expand and leave dv of 32

    TEST_ASSERT_EQUAL_PTR(mem, mem3);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem3, 64 + 4);

    state = kmalloc_debug_getstate();
    
 //   debugDump("dump", heap, 12288);

    test_chunkinfo expected[4] = {
        {USED, 96 + 8, mem3},
        {DV, 32, NULL},
        {USED, 128 + 8, mem2},
        {TC, 4096 - (128 + 8) - 96 - (32 + 8)  - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocExpandSplitBin(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3, *mem4;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(128);
    mem2 = kmalloc(128);
    mem3 = kmalloc(128);
    kfree(mem2); //create a binned chunk of 128 + 8 size
    

    memset(mem, '1', 128 + 4);     
    mem4 = krealloc(mem, 192 + 4);   //increase and split next free chunk

    TEST_ASSERT_EQUAL_PTR(mem, mem4);
    TEST_ASSERT_EACH_EQUAL_INT8('1', mem4, 128 + 4);

    state = kmalloc_debug_getstate();
    
    //debugDump("dump", heap, 12288);

    test_chunkinfo expected[4] = {
        {USED, 192 + 8 , mem4},
        {FREE, 64 + 8 , NULL},
        {USED, 128 + 8, mem3},
        {TC, 4096 - 3 * (128 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocMallocCopy(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(240 + 4);
    mem2 = kmalloc(32);

    memcpy(mem, testdata, 240 + 4);

    mem3 = krealloc(mem, 1024);   //copy and free

    TEST_ASSERT_NOT_NULL(mem3);
    TEST_ASSERT_NOT_EQUAL(mem, mem3);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testdata, mem3, 240 + 4);

    state = kmalloc_debug_getstate();
    
    test_chunkinfo expected[4] = {
        {FREE, 240 + 8 , NULL},
        {USED, 32 + 8 , mem2},
        {USED, 1024 + 8, mem3},
        {TC, 4096 - (240 + 8) - (32 + 8) - (1024 + 8) - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));
}

void test_ReallocMallocCopyExhaustHeap(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);
    libk_set_callback_sbrk(callback_ok);

    mem = kmalloc(240 + 4); //alloc chunk of 240 + 8
    mem2 = kmalloc(32);      //alloc chunk of 32 + 8

    memcpy(mem, testdata, 240 + 4);
    mem3 = krealloc(mem, 4096 - DUMMYSIZE - 248 - 40 - 8);   //copy and free

    TEST_ASSERT_EQUAL(0, cb_ok);
    TEST_ASSERT_NOT_NULL(mem3);
    TEST_ASSERT_NOT_EQUAL(mem, mem3);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testdata, mem3, 240 + 4);

    state = kmalloc_debug_getstate();
    
    test_chunkinfo expected[3] = {
        {FREE, 240 + 8 , NULL},
        {USED, 32 + 8 , mem2},
        {USED, 4096 - DUMMYSIZE - 248 - 40, mem3}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 4096));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, 4096));
    TEST_ASSERT_FALSE(check_bins(heap,  4096, state));  
}

void test_ReallocMallocCopyExtendHeap(void)
{
    struct kmalloc_state state;
    void *mem, *mem2, *mem3;
    libk_init_heap((void *)heap, 4096);
    libk_set_callback_sbrk(callback_ok);

    mem = kmalloc(240 + 4); //alloc chunk of 240 + 8
    mem2 = kmalloc(32);      //alloc chunk of 32 + 8

    memcpy(mem, testdata, 240 + 4);
    mem3 = krealloc(mem, 4096 + 4);   //extend heap, copy and free

    TEST_ASSERT_EQUAL(1, cb_ok);
    TEST_ASSERT_NOT_NULL(mem3);
    TEST_ASSERT_NOT_EQUAL(mem, mem3);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testdata, mem3, 240 + 4);

    state = kmalloc_debug_getstate();
    
    test_chunkinfo expected[4] = {
        {FREE, 240 + 8 , NULL},
        {USED, 32 + 8 , mem2},
        {USED, 4096 + 8, mem3},
        {TC, 8192 - DUMMYSIZE - (240 + 8) - (32 + 8) - (4096 + 8), mem3}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 8192));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 4, state, heap, 8192));
    TEST_ASSERT_FALSE(check_bins(heap,  8192, state));    
}