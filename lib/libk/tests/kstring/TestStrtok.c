#include "unity.h"
#include <kstring.h>

void test_kstrtok_initial(void)
{
    char mem[20] = "==-=abcdef--hi\0-acd";
    char expected[20] = "==-=abcdef\0-hi\0-acd";
    char del[3] = "=-";
    char del2[3] = "=-";

    char *result = kstrtok(mem, del);

    TEST_ASSERT_EQUAL_PTR(&mem[4], result);
    TEST_ASSERT_EQUAL_MEMORY(expected, mem, 20);
    TEST_ASSERT_EQUAL_MEMORY(del2, del, 3);
}

void test_kstrtok_initial_only_delimiters(void)
{
    char mem[20] = "==-=--=-====--\0aacd";
    char expected[20] = "==-=--=-====--\0aacd";
    char del[3] = "=-";
    char del2[3] = "=-";

    char *result = kstrtok(mem, del);

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY(expected, mem, 20);
    TEST_ASSERT_EQUAL_MEMORY(del2, del, 3);
}

void test_kstrtok_initial_no_delimiters(void)
{
    char mem[20] = "test 123456789\0--cd";
    char expected[20] = "test 123456789\0--cd";
    char del[3] = "=-";
    char del2[3] = "=-";

    char *result = kstrtok(mem, del);

    TEST_ASSERT_EQUAL_PTR(&mem, result);
    TEST_ASSERT_EQUAL_MEMORY(expected, mem, 20);
    TEST_ASSERT_EQUAL_MEMORY(del2, del, 3);
}

void test_kstrtok_second(void)
{
    char mem[20] = "==-=ab--def--hi\0acd";
    char expected[20] = "==-=ab\0-def\0-hi\0acd";
    char del[3] = "=-";
    char del2[3] = "=-";

    kstrtok(mem, del);

    char *result = kstrtok(NULL, del);
    TEST_ASSERT_EQUAL_PTR(&mem[8], result);
    TEST_ASSERT_EQUAL_MEMORY(expected, mem, 20);

    result = kstrtok(NULL, del);

    TEST_ASSERT_EQUAL_PTR(&mem[13], result);
    TEST_ASSERT_EQUAL_MEMORY(expected, mem, 20);

    result = kstrtok(NULL, del);

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY(expected, mem, 20);

    result = kstrtok(NULL, del);

    TEST_ASSERT_EQUAL_PTR(NULL, result);
    TEST_ASSERT_EQUAL_MEMORY(expected, mem, 20);

    TEST_ASSERT_EQUAL_MEMORY(del2, del, 3);
}