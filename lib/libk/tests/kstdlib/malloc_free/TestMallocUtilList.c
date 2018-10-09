#include "unity.h"
#include "testsuite.h"
#include <string.h>
#include <stdio.h>

#include <kstdlib.h>
#include "kstdlib/malloc_free/kmalloc.h"
#include "kstdlib/malloc_free/kmalloc_util.h"


void test_ListAddSingle(void)
{
    kmchunk_ptr head = NULL;
    kmchunk chunk1;

    chunk1.header=1;
    kmalloc_dllist_add(&chunk1, &head);
    TEST_ASSERT_EQUAL_PTR(&chunk1, head);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk1.next);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk1.prev);

}

void test_ListAddMultiple(void)
{

    kmchunk_ptr head = NULL;
    kmchunk chunk1;
    kmchunk chunk2;
    kmchunk chunk3;

    kmalloc_dllist_add(&chunk1, &head);
    kmalloc_dllist_add(&chunk2, &head);
    kmalloc_dllist_add(&chunk3, &head);
    TEST_ASSERT_EQUAL_PTR(&chunk3, head);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk1.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk3, chunk2.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk3.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk3, chunk1.next);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk2.next);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk3.next);

}

void test_ListAddEndMultiple(void)
{

    kmchunk_ptr head = NULL;
    kmchunk chunk1;
    kmchunk chunk2;
    kmchunk chunk3;

    kmalloc_dllist_add(&chunk1, &head);     //1
    kmalloc_dllist_add_end(&chunk2, head);  //1 - 2
    kmalloc_dllist_add_end(&chunk3, head);  //1 - 2 - 3
    TEST_ASSERT_EQUAL_PTR(&chunk1, head);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk1.next);
    TEST_ASSERT_EQUAL_PTR(&chunk3, chunk2.next);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk3.next);
    TEST_ASSERT_EQUAL_PTR(&chunk3, chunk1.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk2.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk3.prev);

}

void test_ListRemoveSingle(void)
{

    kmchunk chunk1;
    kmchunk_ptr head = &chunk1;
    kmchunk_ptr result;
    chunk1.next = &chunk1;
    chunk1.prev = &chunk1;

    result = kmalloc_dllist_remove(&head);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL_PTR(&chunk1, result);

}

void test_ListRemoveEmpty(void)
{

    kmchunk_ptr head = NULL;
    kmchunk_ptr result;

    result = kmalloc_dllist_remove(&head);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL_PTR(NULL, result);

}

void test_ListRemoveUntilEmpty(void)
{
    kmchunk chunk1, chunk2, chunk3;

    kmchunk_ptr head = NULL;
    kmchunk_ptr result[3];

    kmalloc_dllist_add(&chunk1, &head);
    kmalloc_dllist_add(&chunk2, &head);
    kmalloc_dllist_add(&chunk3, &head);
    result[0] = kmalloc_dllist_remove(&head);
    result[1] = kmalloc_dllist_remove(&head);
    result[2] = kmalloc_dllist_remove(&head);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL_PTR(&chunk3, result[0]);
    TEST_ASSERT_EQUAL_PTR(&chunk2, result[1]);
    TEST_ASSERT_EQUAL_PTR(&chunk1, result[2]);


}
void test_ListRemoveOneFromMultiple(void)
{

    kmchunk chunk1, chunk2, chunk3;

    kmchunk_ptr head = NULL;
    kmchunk_ptr result;

    kmalloc_dllist_add(&chunk1, &head);
    kmalloc_dllist_add(&chunk2, &head);
    kmalloc_dllist_add(&chunk3, &head);


    result = kmalloc_dllist_remove(&head);

    TEST_ASSERT_EQUAL_PTR(&chunk2, head);
    TEST_ASSERT_EQUAL_PTR(&chunk3, result);

    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk2.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk1.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk2.next);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk1.next);
}

void test_ListRemoveInternalSingle(void){
    kmchunk chunk1;
    kmchunk_ptr head = &chunk1;
    kmchunk_ptr result;
    chunk1.next = &chunk1;
    chunk1.prev = &chunk1;

    result = kmalloc_dllist_remove_intern(&chunk1, &head);

    TEST_ASSERT_EQUAL_PTR(&chunk1, result);
    TEST_ASSERT_EQUAL_PTR(NULL, head);
}



void test_ListRemoveInternalMultipleNotHead(void){
    kmchunk chunk1, chunk2, chunk3;

    kmchunk_ptr head = NULL;
    kmchunk_ptr result;

    kmalloc_dllist_add(&chunk1, &head);
    kmalloc_dllist_add(&chunk2, &head);
    kmalloc_dllist_add(&chunk3, &head);


    result = kmalloc_dllist_remove_intern(&chunk1, &head);

    TEST_ASSERT_EQUAL_PTR(&chunk3, head);
    TEST_ASSERT_EQUAL_PTR(&chunk1, result);

    TEST_ASSERT_EQUAL_PTR(&chunk3, chunk2.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk1.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk3, chunk2.next);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk3.next);    
}

void test_ListRemoveInternalMultipleHead(void){
    kmchunk chunk1, chunk2, chunk3;

    kmchunk_ptr head = NULL;
    kmchunk_ptr result;

    kmalloc_dllist_add(&chunk1, &head);
    kmalloc_dllist_add(&chunk2, &head);
    kmalloc_dllist_add(&chunk3, &head);


    result = kmalloc_dllist_remove_intern(&chunk3, &head);

    TEST_ASSERT_EQUAL_PTR(&chunk2, head);
    TEST_ASSERT_EQUAL_PTR(&chunk3, result);

    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk2.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk1.prev);
    TEST_ASSERT_EQUAL_PTR(&chunk1, chunk2.next);
    TEST_ASSERT_EQUAL_PTR(&chunk2, chunk1.next);    
}
void test_ListRemoveInternalUntilEmpty(void)
{
    kmchunk chunk1, chunk2, chunk3;

    kmchunk_ptr head = NULL;
    kmchunk_ptr result[3];

    kmalloc_dllist_add(&chunk1, &head);
    kmalloc_dllist_add(&chunk2, &head);
    kmalloc_dllist_add(&chunk3, &head);
    result[0] = kmalloc_dllist_remove_intern(&chunk3, &head);
    result[1] = kmalloc_dllist_remove_intern(&chunk1, &head);
    result[2] = kmalloc_dllist_remove_intern(&chunk2, &head);

    TEST_ASSERT_EQUAL_PTR(NULL, head);
    TEST_ASSERT_EQUAL_PTR(&chunk3, result[0]);
    TEST_ASSERT_EQUAL_PTR(&chunk1, result[1]);
    TEST_ASSERT_EQUAL_PTR(&chunk2, result[2]);
}

void test_ListMultipleAddRemove(void){
    
    kmchunk chunk[8];

    kmchunk_ptr head = NULL;
    kmchunk_ptr result, readd;


    /*
        add 0       0 
        add 1       1 - 0
        add 2       2 - 1 - 0
        remove head     1 - 0
        add 3       3 - 1 - 0
        add 4       4 - 3 - 1 - 0
        remove internal 1   4 - 3 - 0   
        add 5               5 - 4 - 3 - 0
        remove internal 0   5 - 4 - 3
        remove internal 3   5 - 4
        add 6               6 - 5 - 4
        remove head         5 - 4
        readd 0             0 - 5 - 4
    */


    kmalloc_dllist_add(&chunk[0], &head);
    kmalloc_dllist_add(&chunk[1], &head);
    kmalloc_dllist_add(&chunk[2], &head);

    result = kmalloc_dllist_remove(&head);

    //check 1 - 0,  result = 2

    TEST_ASSERT_EQUAL_PTR(&chunk[1], head);
    TEST_ASSERT_EQUAL_PTR(&chunk[2], result);
    TEST_ASSERT_EQUAL_PTR(&chunk[1], chunk[0].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[0], chunk[1].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[1], chunk[0].next);
    TEST_ASSERT_EQUAL_PTR(&chunk[0], chunk[1].next);

    kmalloc_dllist_add(&chunk[3], &head);
    kmalloc_dllist_add(&chunk[4], &head);

    result = kmalloc_dllist_remove_intern(&chunk[1], &head);

    //check 4 - 3 - 0, result = 1

    TEST_ASSERT_EQUAL_PTR(&chunk[4], head);
    TEST_ASSERT_EQUAL_PTR(&chunk[1], result);
    TEST_ASSERT_EQUAL_PTR(&chunk[0], chunk[4].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[3], chunk[4].next);
    TEST_ASSERT_EQUAL_PTR(&chunk[4], chunk[3].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[0], chunk[3].next);
    TEST_ASSERT_EQUAL_PTR(&chunk[3], chunk[0].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[4], chunk[0].next);

    kmalloc_dllist_add(&chunk[5], &head);

    readd = kmalloc_dllist_remove_intern(&chunk[0], &head);

    result = kmalloc_dllist_remove_intern(&chunk[3], &head);

    //check readd = 0, result = 3,   5 - 4

    TEST_ASSERT_EQUAL_PTR(&chunk[5], head);
    TEST_ASSERT_EQUAL_PTR(&chunk[3], result);
    TEST_ASSERT_EQUAL_PTR(&chunk[0], readd);

    TEST_ASSERT_EQUAL_PTR(&chunk[4], chunk[5].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[4], chunk[5].next);
    TEST_ASSERT_EQUAL_PTR(&chunk[5], chunk[4].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[5], chunk[4].next);  

    kmalloc_dllist_add(&chunk[6], &head);

    result = kmalloc_dllist_remove(&head);

    kmalloc_dllist_add(readd, &head);

    //check 0 - 5 - 4, result = 6
    
    TEST_ASSERT_EQUAL_PTR(&chunk[0], head);
    TEST_ASSERT_EQUAL_PTR(&chunk[6], result);
    TEST_ASSERT_EQUAL_PTR(&chunk[4], chunk[0].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[5], chunk[0].next);
    TEST_ASSERT_EQUAL_PTR(&chunk[0], chunk[5].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[4], chunk[5].next);
    TEST_ASSERT_EQUAL_PTR(&chunk[5], chunk[4].prev);
    TEST_ASSERT_EQUAL_PTR(&chunk[0], chunk[4].next);


}






