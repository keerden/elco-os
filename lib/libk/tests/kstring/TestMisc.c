#include "unity.h"
#include <kstring.h>


void test_kstrlen(void)
{
    char text[18] = "abcdefgh\0ijklmnop";
    char expected[18] = "abcdefgh\0ijklmnop";


    size_t result = kstrlen(text);

    TEST_ASSERT_EQUAL(8, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, text, 18);
}

void test_kstrlen_empty(void)
{
    char text[18] = "\0abcdefghijklmnop";
    char expected[18] = "\0abcdefghijklmnop";


    size_t result = kstrlen(text);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, text, 18);
}

void test_kmemset(void){
    char text[18] = "abcdefghijklmnop";
    char expected[18] = "abcde-----klmnop";

    void* result = kmemset(&text[5], '-', 5);

    TEST_ASSERT_EQUAL_PTR(&text[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, text, 18);  
}

void test_kmemset_zero(void){
    char text[18] = "abcdefghijklmnop";
    char expected[18] = "abcdefghijklmnop";

    void* result = kmemset(&text[5], '-', 0);

    TEST_ASSERT_EQUAL_PTR(&text[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, text, 18);  
}

void test_kmemset_convert_to_unsigned_char(void){
    char mem[8] = {1,2,3,4,5,6,7,8};
    char expected[8] = {1,2,3,0x34,0x34,6,7,8};;

    void* result = kmemset(&mem[3], 0x1234, 2);

    TEST_ASSERT_EQUAL_PTR(&mem[3], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 8);  
}