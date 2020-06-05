#include "kernel.h"
#include "task.h"

#include <libk.h>
#include <kstring.h>
#include <kstdlib.h>

#define TASK_COUNT 10



struct task *next_task;
struct task *current_task;
struct task ptable[TASK_COUNT];
static int pid_count;

int nested_intr;

void multitasking_init(void)
{

    for(int i = 0; i < TASK_COUNT; i++){
        ptable[i].pid = -1;
        ptable[i].context = NULL;
        ptable[i].stack_space = NULL;
        ptable[i].stack_size = 0;
        ptable[i].page_dir = (phys_bytes) kvirt_to_phys(boot_page_directory);
    }

    pid_count = 0;
    next_task = NULL;
    current_task = NULL;
    nested_intr = 0;

}


struct task *create_kernel_task(void * entry_point, size_t stack_size)
{
    struct task *tsk = NULL;
    void *stack;
    vir_bytes sp;
    intr_stack_t *context;

    for(int i = 0; i < TASK_COUNT; i++)
    {
        if(ptable[i].pid < 0){
            tsk = &ptable[i];
            break;
        }
    }

    if(tsk != NULL) {
        stack = kmalloc(stack_size);
        if(stack) {
            tsk->pid = pid_count++;
            tsk->stack_space = stack;
            tsk->stack_size = stack_size;
            
            //TODO: move to arch file
            sp = (vir_bytes) stack + stack_size;
            context = ((intr_stack_t *) sp) - 1;
            kmemset(context, 0, sizeof(intr_stack_t));

            context->cs = GDT_RING0_CODE;
            context->ds = GDT_RING0_DATA;
            context->es = GDT_RING0_DATA;
            context->fs = GDT_RING0_DATA;
            context->gs = GDT_RING0_DATA;
            context->ss = GDT_RING0_DATA;

            context->ebp = sp;
            context->eip = (uint32_t) entry_point;
            //TODO: replace 'magic' number
            context->eflags = 0x0202u; //0x0202 for ints enabled, 0x3202 for usermode     

            tsk->context = context;

        } else {
            tsk = NULL;
        }
    }

    return tsk;
}

void pick_task(struct task *next) {
    next_task = next;
}




