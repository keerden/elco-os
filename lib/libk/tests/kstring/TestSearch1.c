#include "unity.h"
#include <kstring.h>


void test_kmemchr(void)
{
    char mem[10] = {0,1,2,3,4,5,6,7,8,9};
    char expected[10] = {0,1,2,3,4,5,6,7,8,9};

    void* result = kmemchr(mem, 5, 10);

    TEST_ASSERT_EQUAL_PTR(&mem[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kmemchr_not_found(void)
{
    char mem[10] = {0,1,2,3,4,5,6,7,8,9};
    char expected[10] = {0,1,2,3,4,5,6,7,8,9};

    void* result = kmemchr(mem, 8, 5);

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kmemchr_zero(void)
{
    char mem[10] = {0,1,2,3,4,5,6,7,8,9};
    char expected[10] = {0,1,2,3,4,5,6,7,8,9};

    void* result = kmemchr(mem, 0, 0);

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kmemchr_converts_unsigned_char(void)
{
    char mem[10] = {0,1,2,3,4,5,6,7,8,9};
    char expected[10] = {0,1,2,3,4,5,6,7,8,9};

    void* result = kmemchr(mem, 0x5005, 10);

    TEST_ASSERT_EQUAL_PTR(&mem[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kstrchr(void)
{
    char mem[10] = "test12341";
    char expected[10] = "test12341";

    char* result = kstrchr(mem, '1');

    TEST_ASSERT_EQUAL_PTR(&mem[4], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kstrchr_not_found(void)
{
    char mem[15] = "test12345\0abcd";
    char expected[15] = "test12345\0abcd";

    char* result = kstrchr(mem, 'a');

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kstrchr_null_char(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";

    char* result = kstrchr(mem, '\0');

    TEST_ASSERT_EQUAL_PTR(&mem[9], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}
void test_kstrchr_null_char_empty(void)
{
    char mem[10] = "\0abcdefgh";
    char expected[10] = "\0abcdefgh";

    char* result = kstrchr(mem, '\0');

    TEST_ASSERT_EQUAL_PTR(&mem[0], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}
void test_kstrchr_empty_not_found(void)
{
    char mem[10] = "\0abcdefgh";
    char expected[10] = "\0abcdefgh";

    char* result = kstrchr(mem, 'a');

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
}

void test_kstrchr_converts_unsigned_char(void)
{
    char mem[11] = "0123456789";
    char expected[11] = "0123456789";
    int find = 0x1200 + '5';

    char* result = kstrchr(mem, find);

    TEST_ASSERT_EQUAL_PTR(&mem[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 11);
}

void test_kstrcspn_s2_single_char(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[2] = "2";
    char expected2[2] = "2";
    size_t result = kstrcspn(mem, keys);

    TEST_ASSERT_EQUAL(5, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 2);
}

void test_kstrcspn_s2_multiple_char(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[3] = "2s";
    char expected2[3] = "2s";
    size_t result = kstrcspn(mem, keys);

    TEST_ASSERT_EQUAL(2, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 3);
}

void test_kstrcspn_s2_empty(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[1] = "";
    char expected2[1] = "";
    size_t result = kstrcspn(mem, keys);

    TEST_ASSERT_EQUAL(9, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 1);
}

void test_kstrcspn_s1_empty(void)
{
    char mem[1] = "";
    char expected[1] = "";
    char keys[4] = "abc";
    char expected2[4] = "abc";
    size_t result = kstrcspn(mem, keys);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 1);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 4);
}

void test_kstrcspn_not_found(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[4] = "abc";
    char expected2[4] = "abc";
    size_t result = kstrcspn(mem, keys);

    TEST_ASSERT_EQUAL(9, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 4);
}

void test_kstrpbrk_s2_single_char(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[2] = "2";
    char expected2[2] = "2";
    char* result = kstrpbrk(mem, keys);

    TEST_ASSERT_EQUAL_PTR(&mem[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 2);
}

void test_kstrpbrk_s2_multiple_char(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[3] = "2s";
    char expected2[3] = "2s";
    char* result = kstrpbrk(mem, keys);

    TEST_ASSERT_EQUAL_PTR(&mem[2], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 3);
}

void test_kstrpbrk_s2_empty(void)
{
    char mem[10] = "test12345";
    char expected[10] = "test12345";
    char keys[1] = "";
    char expected2[1] = "";
    char* result = kstrpbrk(mem, keys);

    TEST_ASSERT_EQUAL(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 1);
}

void test_kstrpbrk_s1_empty(void)
{
    char mem[1] = "";
    char expected[1] = "";
    char keys[4] = "abc";
    char expected2[4] = "abc";
    char* result = kstrpbrk(mem, keys);

    TEST_ASSERT_EQUAL(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 1);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 4);
}

void test_kstrpbrk_not_found(void)
{
    char mem[10] = "te\0t12345";
    char expected[10] = "te\0t12345";
    char keys[4] = "123";
    char expected2[4] = "123";
    char* result = kstrpbrk(mem, keys);

    TEST_ASSERT_EQUAL(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, mem, 10);
    TEST_ASSERT_EQUAL_MEMORY (expected2, keys, 4);
}
