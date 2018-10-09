#include "unity.h"
#include <kstring.h>


void test_kmemcmp_equal(void)
{
    char a[10] = {0,1,2,3,4,5,6,7,8,9};
    char b[10] = {0,1,2,3,4,5,6,7,8,9};

    char cmp[10] = {0,1,2,3,4,5,6,7,8,9};

    int result = kmemcmp(a, b, 10);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmp, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmp, b, 10);
}

void test_kmemcmp_less(void)
{
    char a[10] = {0,1,2,3,4,4,6,7,8,9};
    char b[10] = {0,1,2,3,4,5,6,7,8,9};

    char cmpa[10] = {0,1,2,3,4,4,6,7,8,9};
    char cmpb[10] = {0,1,2,3,4,5,6,7,8,9};

    int result = kmemcmp(a, b, 10);

    TEST_ASSERT_LESS_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kmemcmp_greater(void)
{
    char a[10] = {0,1,2,3,4,5,6,7,8,10};
    char b[10] = {0,1,2,3,4,5,6,7,8,9};

    char cmpa[10] = {0,1,2,3,4,5,6,7,8,10};
    char cmpb[10] = {0,1,2,3,4,5,6,7,8,9};

    int result = kmemcmp(a, b, 10);

    TEST_ASSERT_GREATER_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kmemcmp_zero_length(void)
{
    char a[10] = {1,1,2,3,4,5,6,7,8,10};
    char b[10] = {0,1,2,3,4,5,6,7,8,9};

    char cmpa[10] = {1,1,2,3,4,5,6,7,8,10};
    char cmpb[10] = {0,1,2,3,4,5,6,7,8,9};

    int result = kmemcmp(a, b, 0);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrcmp_equal(void)
{
    char a[10] = "test12345";
    char b[10] = "test12345";

    char cmp[10] = "test12345";

    int result = kstrcmp(a, b);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmp, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmp, b, 10);
}

void test_kstrcmp_less(void)
{
    char a[10] = "test12245";
    char b[10] = "test12345";

    char cmpa[10] = "test12245";
    char cmpb[10] = "test12345";

    int result = kstrcmp(a, b);

    TEST_ASSERT_LESS_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrcmp_greater(void)
{
    char a[10] = "test12545";
    char b[10] = "test12345";

    char cmpa[10] = "test12545";
    char cmpb[10] = "test12345";

    int result = kstrcmp(a, b);

    TEST_ASSERT_GREATER_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrcmp_zero_length(void)
{
    char a[10] = "\0abcdefgh";
    char b[10] = "\0qqcdefgh";

    char cmpa[10] = "\0abcdefgh";
    char cmpb[10] = "\0qqcdefgh";

    int result = kstrcmp(a, b);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrncmp_equal(void)
{
    char a[10] = "test12345";
    char b[10] = "test12345";

    char cmp[10] = "test12345";

    int result = kstrncmp(a, b, 5);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmp, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmp, b, 10);
}

void test_kstrncmp_less(void)
{
    char a[10] = "test12245";
    char b[10] = "test12345";

    char cmpa[10] = "test12245";
    char cmpb[10] = "test12345";

    int result = kstrncmp(a, b, 10);

    TEST_ASSERT_LESS_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrncmp_greater(void)
{
    char a[10] = "test12545";
    char b[10] = "test12345";

    char cmpa[10] = "test12545";
    char cmpb[10] = "test12345";

    int result = kstrncmp(a, b, 10);

    TEST_ASSERT_GREATER_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrncmp_zero_length(void)
{
    char a[10] = "abcdefghi";
    char b[10] = "qqcdefghi";

    char cmpa[10] = "abcdefghi";
    char cmpb[10] = "qqcdefghi";

    int result = kstrncmp(a, b, 0);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrncmp_both_shorter_than_n(void)
{
    char a[10] = "abcd\0fghi";
    char b[10] = "abcd\0gghi";

    char cmpa[10] = "abcd\0fghi";
    char cmpb[10] = "abcd\0gghi";

    int result = kstrncmp(a, b, 10);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}
void test_kstrncmp_a_shorter_than_n(void)
{
    char a[10] = "ab\0defghi";
    char b[10] = "abcdefghi";

    char cmpa[10] = "ab\0defghi";
    char cmpb[10] = "abcdefghi";

    int result = kstrncmp(a, b, 10);

    TEST_ASSERT_LESS_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrncmp_b_shorter_than_n(void)
{
    char b[10] = "ab\0defghi";
    char a[10] = "abcdefghi";

    char cmpb[10] = "ab\0defghi";
    char cmpa[10] = "abcdefghi";

    int result = kstrncmp(a, b, 10);

    TEST_ASSERT_GREATER_THAN(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

void test_kstrncmp_shorter_than_n(void)
{
    char a[10] = "abcd\0fghi";
    char b[10] = "abcd\0gghi";

    char cmpa[10] = "abcd\0fghi";
    char cmpb[10] = "abcd\0gghi";

    int result = kstrncmp(a, b, 10);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}


void test_kstrncmp_longer_than_n(void)
{
    char a[10] = "abcdefghi";
    char b[10] = "abcdegghi";

    char cmpa[10] = "abcdefghi";
    char cmpb[10] = "abcdegghi";

    int result = kstrncmp(a, b, 5);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_MEMORY (cmpa, a, 10);
    TEST_ASSERT_EQUAL_MEMORY (cmpb, b, 10);
}

