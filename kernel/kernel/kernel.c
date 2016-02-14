#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_main(void)
{
	
 	terminal_print_logo();
 	printf("Elco-Os: Hello World! s:%i, u:%u, o:%o, x:%x, p:%p, %%teken \n", -12345, 6789, 07712, 0xabcd, kernel_main);
 	printf("1	2	3	4	5	6	7	8	9	11	12	13	14	15	16	17	18	19\n");
 	printf("1\t2\t3\t4\t5\t6\t7\t8\t9\t11\t12\t13\t14\t15\t16\t17\t18\t19\t20\t21\t22\n");
 	printf("Hierna is een teken gewist: 12\b345\n");
 	printf("01234567890123456789012345678901234567890123456789012345678901234567890123456789\n");
	printf("\bA\n");
   printf("12345 carriage return\r6\n");
 	
	
}
