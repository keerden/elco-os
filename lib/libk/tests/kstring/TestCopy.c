#include "unity.h"
#include <kstring.h>


void test_kmemcpy(void)
{
    char destination[18] = "testing...1234567";
    char source[14] = "---success---";

    char expected[18] = "testing...success";
    char sourcecpy[14] = "---success---";

    void* result = kmemcpy(&destination[10], &source[3], 7);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 18);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 14);
}

void test_kmemcpy_zero(void)
{
    char destination[18] = "testing...1234567";
    char source[14] = "---success---";

    char expected[18] = "testing...1234567";
    char sourcecpy[14] = "---success---";

    void* result = kmemcpy(&destination[10], &source[3], 0);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 18);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 14);
}

void test_kmemmove_non_overlapping(void)
{
    char destination[18] = "testing...1234567";
    char source[14] = "---success---";

    char expected[18] = "testing...success";
    char sourcecpy[14] = "---success---";

    void* result = kmemmove(&destination[10], &source[3], 7);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 18);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 14);
}

void test_kmemmove_overlap_left(void)
{
    char buffer[37] = "0123456789abcdefghijklmnopqrstuvwxyz";

    char expected[37] = "01234abcdefghijfghijklmnopqrstuvwxyz";

    void* result = kmemmove(&buffer[5], &buffer[10], 10);

    TEST_ASSERT_EQUAL_PTR(&buffer[5], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, buffer, 37);
}

void test_kmemmove_overlap_right(void)
{
    char buffer[37] = "0123456789abcdefghijklmnopqrstuvwxyz";

    char expected[37] = "012345678956789abcdeklmnopqrstuvwxyz";

    void* result = kmemmove(&buffer[10], &buffer[5], 10);

    TEST_ASSERT_EQUAL_PTR(&buffer[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, buffer, 37);
}

void test_kstrcpy(void) {
   char destination[18] = "testing...1234567";
    char source[7] = "OK\0###";

    char expected[18] = "testing...OK@4567";
    expected[12] = '\0';
    char sourcecpy[7] = "OK\0###";

    void* result = kstrcpy(&destination[10], source);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 18);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 7);
}

void test_kstrcpy_empty(void){
   char destination[18] = "testing...1234567";
    char source[5] = "\0###";

    char expected[18] = "testing...@234567";
    expected[10] = '\0';
    char sourcecpy[5] = "\0###";

    void* result = kstrcpy(&destination[10], source);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 18);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 5);
}

void test_kstrncpy(void) {
    char destination[19] = "testing...1234567!";
    char source[14] = "---success---";

    char expected[19] = "testing...success!";
    char sourcecpy[14] = "---success---";

    void* result = kstrncpy(&destination[10], &source[3], 7);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 19);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 14);
}

void test_kstrncpy_short(void) {
    char destination[18] = "testing...1234567";
    char source[10] = "--OK\0QQQQ";

    char expected[18] = "testing...OK\0\0\0\0\0";
    char sourcecpy[10] = "--OK\0QQQQ";

    void* result = kstrncpy(&destination[10], &source[2], 7);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 18);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 10);
}

void test_kstrncpy_zero(void){
   char destination[18] = "testing...1234567";
    char source[5] = "1234";

    char expected[18] = "testing...1234567";
    char sourcecpy[5] = "1234";

    void* result = kstrncpy(&destination[10], source, 0);

    TEST_ASSERT_EQUAL_PTR(&destination[10], result);
    TEST_ASSERT_EQUAL_MEMORY (expected, destination, 18);
    TEST_ASSERT_EQUAL_MEMORY (sourcecpy, source, 5);
}