#include "unity.h"
#include <kstring.h>


void test_kstrcat(void)
{
    char destination[21] = "testing\0..1234567890";
    char source[10] = "..success";

    char expected[21] = "testing..success@890";
    expected[16] = '\0';
    char sourcecpy[10] = "..success";

    void* result = kstrcat(destination, source);

    TEST_ASSERT_EQUAL_PTR(destination, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 21);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 10);
}

void test_kstrcat_src_empty(void)
{
    char destination[21] = "testing\0..1234567890";
    char source[10] = "\0.success";

    char expected[21] = "testing\0..1234567890";
    char sourcecpy[10] = "\0.success";  // = "" but with some garbage after it

    void* result = kstrcat(destination, source);

    TEST_ASSERT_EQUAL_PTR(destination, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 21);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 10);
}

void test_kstrcat_dst_empty(void)
{
    char destination[21] = "\0testing...123456789"; // = "" but with some garbage after it
    char source[10] = "..success";

    char expected[21] = "..success\0.123456789";
    char sourcecpy[10] = "..success";

    void* result = kstrcat(destination, source);

    TEST_ASSERT_EQUAL_PTR(destination, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 21);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 10);
}

void test_kstrncat(void)
{
    char destination[21] = "testing\0..1234567890";
    char source[10] = "..success";

    char expected[21] = "testing..success@890";
    expected[16] = '\0';
    char sourcecpy[10] = "..success";

    void* result = kstrncat(destination, source, 10);

    TEST_ASSERT_EQUAL_PTR(destination, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 21);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 10);
}

void test_kstrncat_larger(void)
{
    char destination[21] = "testing\0..1234567890";
    char source[10] = "--success";

    char expected[21] = "testing--@1234567890";
    expected[9] = '\0';
    char sourcecpy[10] = "--success";

    void* result = kstrncat(destination, source, 3);

    TEST_ASSERT_EQUAL_PTR(destination, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 21);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 10);
}

void test_kstrncat_smaller(void)
{
    char destination[21] = "testing\0..1234567890";
    char source[10] = "--OK";

    char expected[21] = "testing--OK@34567890";
    expected[11] = '\0';
    char sourcecpy[10] = "--OK";

    void* result = kstrncat(destination, source, 10);

    TEST_ASSERT_EQUAL_PTR(destination, result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 21);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 10);
}