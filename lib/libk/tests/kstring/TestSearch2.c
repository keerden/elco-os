#include "unity.h"
#include <kstring.h>





void test_kstrrchr(void)
{
    char mem[10] = "test12312";
    char expected[10] = "test12312";

    char* result = kstrrchr(mem, '1');

    TEST_ASSERT_EQUAL_PTR(&mem[7], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kstrrchr_not_found(void)
{
    char mem[15] = "test12345\0abcd";
    char expected[15] = "test12345\0abcd";

    char* result = kstrrchr(mem, 'a');

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kstrrchr_null_char(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";

    char* result = kstrrchr(mem, '\0');

    TEST_ASSERT_EQUAL_PTR(&mem[9], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}
void test_kstrrchr_null_char_empty(void)
{
    char mem[10] = "\0abcdefgh";
    char expected[10] = "\0abcdefgh";

    char* result = kstrrchr(mem, '\0');

    TEST_ASSERT_EQUAL_PTR(&mem[0], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}
void test_kstrrchr_empty_not_found(void)
{
    char mem[10] = "\0abcdefgh";
    char expected[10] = "\0abcdefgh";

    char* result = kstrrchr(mem, 'a');

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kstrrchr_converts_unsigned_char(void)
{
    char mem[11] = "0123456789";
    char expected[11] = "0123456789";
    int find = 0x1200 + '5';

    char* result = kstrrchr(mem, find);

    TEST_ASSERT_EQUAL_PTR(&mem[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 11);
}

void test_kstrspn_s2_single_char(void)
{
    char mem[10] = "aaaa12345";
    char expected[10] = "aaaa12345";
    char keys[2] = "a";
    char expected2[2] = "a";
    size_t result = kstrspn(mem, keys);

    TEST_ASSERT_EQUAL(4, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 2);
}

void test_kstrspn_s2_multiple_char(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[5] = "2ste";
    char expected2[5] = "2ste";
    size_t result = kstrspn(mem, keys);

    TEST_ASSERT_EQUAL(4, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 5);
}

void test_kstrspn_s2_empty(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[1] = "";
    char expected2[1] = "";
    size_t result = kstrspn(mem, keys);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 1);
}

void test_kstrspn_s1_empty(void)
{
    char mem[1] = "";
    char expected[1] = "";
    char keys[4] = "abc";
    char expected2[4] = "abc";
    size_t result = kstrspn(mem, keys);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 1);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 4);
}

void test_kstrspn_not_found(void)
{
    char mem[10] = "tes\0t1234";
    char expected[10] = "tes\0t1234";
    char keys[4] = "123";
    char expected2[4] = "123";
    size_t result = kstrspn(mem, keys);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 4);
}


void test_kstrstr(void)
{
    char mem[15] = "this is a test";
    char expected[15] = "this is a test";
    char keys[3] = "is";
    char expected2[3] = "is";
    char *result = kstrstr(mem, keys);

    TEST_ASSERT_EQUAL_PTR(&mem[2], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 15);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 3);    
}

void test_kstrstr_almost_double(void)
{
    char mem[15] = "this is a test";
    char expected[15] = "this is a test";
    char keys[5] = "is a";
    char expected2[5] = "is a";
    char *result = kstrstr(mem, keys);

    TEST_ASSERT_EQUAL_PTR(&mem[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 15);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 5);    
}

void test_kstrstr_at_end(void)
{
    char mem[15] = "this is a test";
    char expected[15] = "this is a test";
    char keys[5] = "test";
    char expected2[5] = "test";
    char *result = kstrstr(mem, keys);

    TEST_ASSERT_EQUAL_PTR(&mem[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 15);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 5);    
}

void test_kstrstr_not_found(void)
{
    char mem[15] = "this\0is a test";
    char expected[15] = "this\0is a test";
    char keys[4] = "is ";
    char expected2[4] = "is ";
    char *result = kstrstr(mem, keys);

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 15);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 4);    
}

void test_kstrstr_s2_empty(void)
{
    char mem[15] = "this is a test";
    char expected[15] = "this is a test";
    char keys[1] = "";
    char expected2[1] = "";
    char *result = kstrstr(mem, keys);

    TEST_ASSERT_EQUAL_PTR(mem, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 15);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 1);    
}

void test_kstrstr_both_empty(void)
{
    char mem[1] = "";
    char expected[1] = "";
    char keys[1] = "";
    char expected2[1] = "";
    char *result = kstrstr(mem, keys);

    TEST_ASSERT_EQUAL_PTR(mem, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 1);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 1);    
}