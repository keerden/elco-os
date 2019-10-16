#include "kernel.h"
#include <libk.h>
#include <kstring.h>
#include <kstdlib.h>

struct process *current_proc;
struct process ptable[5];
static int pid_count;

void task1(void);
void task2(void);


typedef struct task_stack
{
    uint32_t data[32];
    uint32_t ebp, edi, esi, ebx;
    uint32_t eip;
} task_stack_t; 

void multitasking_init(void)
{

/*  TODO:
    - method for setting up new PD
    - interface to switch task in i386 ( switch CR3 and update ESP0 in TSS)
    - method to print on specific position on screen (to prevent race conditions)

*/
    for(int i = 0; i < 5; i++){
        ptable[i].pid = -1;
        ptable[i].stack_pointer = 0;
        ptable[i].page_dir = kvirt_to_phys(boot_page_directory);
    }

    ptable[0].pid = 0;
    pid_count = 1;
    current_proc = &ptable[0];




    task_create(task2);
    task1();
}

void task_create(void * starting_point){
    int i;
    task_stack_t *stack;

    for(i = 0; i < 5 && ptable[i].pid >= 0; i++);
    
    if(i == 5)
        return;

    ptable[i].pid = pid_count++;
    
    stack = kmalloc(sizeof(task_stack_t));

    //setup stack
    stack->eip = (uint32_t) starting_point;
    ptable[i].stack_pointer = &(stack->ebp);

}


void task1(void){
    while(1){
        kprintf("Task1 \n");
        switch_to_task(&ptable[1]);
    }
}
void task2(void){
    while(1){
        kprintf("Task2 \n");
        switch_to_task(&ptable[0]);
    }
}