/*
 * loc_inc.h - local included header file for the stdio library
 */

#include <stdio.h>
#include <stdarg.h>

char* i_convert (char* s, unsigned long long val, int base, int digits);
size_t i_print(int i);
