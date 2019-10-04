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


void test_MallocMaxSize(void)   //allocate all space within the heap
{
    struct kmalloc_state state;
    void *mem;
    libk_set_callback_sbrk(callback_ok);
    libk_init_heap((void *)heap, 512);

    mem = kmalloc(512 + 4 - 8 - DUMMYSIZE);
    TEST_ASSERT_NOT_NULL(mem);
    TEST_ASSERT_EQUAL(0, cb_ok);

    memset(mem, '1', 512 + 4 - 8 - DUMMYSIZE);

    state = kmalloc_debug_getstate();
    TEST_ASSERT_EQUAL(512, state.heap_size);
    

    test_chunkinfo expected[2] = {
        {USED, 512 - DUMMYSIZE, mem},
        {TC, 0, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 512));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 512));
    TEST_ASSERT_FALSE(check_bins(heap,  512, state));
    
    //debugDump("exhausted top", heap, 512);

    
    //now check if free correctly increases the empty top chunk

    kfree(mem);

    //debugDump("free exhausted top", heap, 512);

    TEST_ASSERT_EQUAL(0, cb_ok);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected2[1] = {
        {TC, 512 - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 512));
    TEST_ASSERT_FALSE(check_heap_layout(expected2, 1, state, heap, 512));
    TEST_ASSERT_FALSE(check_bins(heap,  512, state));


}

void test_MallocOutOfMem(void)  //allocate more than heapsize and more than max mem
{
    struct kmalloc_state state;
    void *mem;
    libk_set_callback_sbrk(callback_fail);
    libk_init_heap((void *)heap, 512);

    mem = kmalloc(580);
    TEST_ASSERT_NULL(mem);
    TEST_ASSERT_EQUAL(1, cb_fail);

    state = kmalloc_debug_getstate();
    TEST_ASSERT_EQUAL(512, state.heap_size);

    test_chunkinfo expected[1] = {
        {TC, 512 - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 512));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 1, state, heap, 512));
    TEST_ASSERT_FALSE(check_bins(heap,  512, state));
}

void test_MallocOverLimit(void)  //allocate more than heapsize, but less than max mem
{
    struct kmalloc_state state;
    void *mem;
    libk_set_callback_sbrk(callback_ok);
    libk_init_heap((void *)heap, 4096);

    mem = kmalloc(4200);
    TEST_ASSERT_NOT_NULL(mem);
    TEST_ASSERT_EQUAL(1, cb_ok);

    memset(mem, '1', 4200);

    state = kmalloc_debug_getstate();
    TEST_ASSERT_EQUAL(8192, state.heap_size);
    test_chunkinfo expected[2] = {
        {USED, 4200 + 8, mem},
        {TC, 8192 - 4200 - 8 -  DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 8192));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 8192));
    TEST_ASSERT_FALSE(check_bins(heap,  8192, state));
    
}


void test_FreeNULLptr(void)
{
  struct kmalloc_state state;
    void *mem;
    libk_set_callback_abort(callback_abort);
    libk_init_heap((void *)heap, 512);
    mem = kmalloc(32);

    kfree(NULL);

    TEST_ASSERT_EQUAL(0, cb_abort);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected[2] = {
        {USED, 32 + 8, mem},
        {TC, 512 - DUMMYSIZE - (32 + 8), NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 512));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 512));
    TEST_ASSERT_FALSE(check_bins(heap,  512, state));
}

void test_FreeOutsideHeap(void)
{
  struct kmalloc_state state;
    void *mem;
    libk_set_callback_abort(callback_abort);
    libk_init_heap((void *)heap, 512);
    mem = kmalloc(32);

    kfree(&heap[512]);

    TEST_ASSERT_EQUAL(1, cb_abort);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected[2] = {
        {USED, 32 + 8, mem},
        {TC, 512 - DUMMYSIZE - (32 + 8), NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 512));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 512));
    TEST_ASSERT_FALSE(check_bins(heap,  512, state));
}

void test_FreeUnusedChunk(void)
{
  struct kmalloc_state state;
    void *mem1, *mem2;
    libk_set_callback_abort(callback_abort);
    libk_init_heap((void *)heap, 512);
    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    kfree(mem1);

    kfree(mem1);

    TEST_ASSERT_EQUAL(1, cb_abort);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected[3] = {
        {FREE, 32 + 8, NULL},
        {USED, 32 + 8, mem2},
        {TC, 512 - DUMMYSIZE - (64 + 16), NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 512));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 3, state, heap, 512));
    TEST_ASSERT_FALSE(check_bins(heap,  512, state));
}

void test_FreeWrongPointer(void)
{
   struct kmalloc_state state;
    uint8_t *mem;
    libk_set_callback_abort(callback_abort);
    libk_init_heap((void *)heap, 512);
    mem = kmalloc(32);

    kfree(&mem[1]);

    TEST_ASSERT_EQUAL(1, cb_abort);

    state = kmalloc_debug_getstate();

    test_chunkinfo expected[2] = {
        {USED, 32 + 8, mem},
        {TC, 512 - DUMMYSIZE - (32 + 8), NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 512));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 512));
    TEST_ASSERT_FALSE(check_bins(heap,  512, state));
}


void test_FreeCorruption(void)
{
    uint8_t *mem1, *mem2;
    kmchunk_ptr c = (kmchunk_ptr) heap;


    libk_set_callback_abort(callback_abort);
    libk_init_heap((void *)heap, 512);
    mem1 = kmalloc(32);
    mem2 = kmalloc(32);
    kfree(mem1);

    c->header = 0xF0;
    kfree(mem2);

    TEST_ASSERT_EQUAL(1, cb_abort);

}

void test_FreeSchrinkTop(void)
{
    struct kmalloc_state state;
    void *mem;
    libk_set_callback_sbrk(callback_ok);
    libk_init_heap((void *)heap, 4096U);

    mem = kmalloc(HEAP_SHRINK_TRESHOLD + 4096U - DUMMYSIZE + 4);
    memset(mem, '#', HEAP_SHRINK_TRESHOLD + 4096U - DUMMYSIZE+ 4);
 //   printf("size %x \n", HEAP_SHRINK_TRESHOLD + 4096U - DUMMYSIZE + 4);
 //   debugDump("dump", heap, 12288);
    kfree(mem);

 //   debugDump("dump free", heap, 12288);


    state = kmalloc_debug_getstate();
    TEST_ASSERT_EQUAL(2, cb_ok);
    TEST_ASSERT_EQUAL(-((HEAP_SHRINK_TRESHOLD + 8) & ~(HEAP_SIZE_ALIGN - 1))  , increment);
    TEST_ASSERT_EQUAL(8192U, state.heap_size);

    test_chunkinfo expected[1] = {
        {TC, 8192U - DUMMYSIZE, NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 8192U));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 1, state, heap, 8192U));
    TEST_ASSERT_FALSE(check_bins(heap,  8192U, state));
}

void test_FreeSchrinkEmptyTop(void)
{
    struct kmalloc_state state;
    void *mem1, *mem2;
    libk_set_callback_sbrk(callback_ok);
    libk_init_heap((void *)heap, 4096U);

    mem1 = kmalloc(4096U -8 - DUMMYSIZE + 4);
    memset(mem1, 'A', 4096U -8 - DUMMYSIZE + 4);
    mem2 = kmalloc(HEAP_SHRINK_TRESHOLD + 4);

    kfree(mem2);

    state = kmalloc_debug_getstate();
    TEST_ASSERT_EQUAL(2, cb_ok);
    TEST_ASSERT_EQUAL(-4096, increment);
    TEST_ASSERT_EQUAL(8192U, state.heap_size);

    test_chunkinfo expected[2] = {
        {USED, 4096U - DUMMYSIZE, mem1},
        {TC, 8192U - (4096U - DUMMYSIZE) - DUMMYSIZE , NULL}};

    TEST_ASSERT_FALSE(check_heap_integrity(heap, 8192U));
    TEST_ASSERT_FALSE(check_heap_layout(expected, 2, state, heap, 8192U));
    TEST_ASSERT_FALSE(check_bins(heap,  8192U, state));
}

