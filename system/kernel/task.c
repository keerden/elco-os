#include "kernel.h"
#include "task.h"

#include <libk.h>
#include <kstring.h>
#include <kstdlib.h>

#define TASK_COUNT 10

int nested_intr;

struct task ptable[TASK_COUNT];
struct task *next_task;
struct task *current_task;

static int pid_count;
static struct task *queue_head;
static struct task *queue_tail;

static void printQ(void);


void multitasking_init(void)
{

    for(int i = 0; i < TASK_COUNT; i++){
        ptable[i].pid = -1;
        ptable[i].context = NULL;
        ptable[i].stack_space = NULL;
        ptable[i].stack_size = 0;
        ptable[i].page_dir = (phys_bytes) kvirt_to_phys(boot_page_directory);
        ptable[i].timer_next = NULL;
    }

    next_task = NULL;
    current_task = NULL;
    nested_intr = 0;

    pid_count = 0;
    queue_head = queue_tail = NULL;

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

            tsk->next_ready = NULL;
            
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
            tsk->timer_next = NULL;

        } else {
            tsk = NULL;
        }
    }

    return tsk;
}

void pick_task(void){
    next_task = queue_head;
}

void enqueue_task(struct task *tsk){
    tsk->next_ready = NULL;
    if(queue_head == NULL) {
        queue_head = queue_tail = tsk;
    } else {
        queue_tail->next_ready = tsk;
        queue_tail = tsk;
    }
    pick_task();
}
void dequeue_task(struct task *tsk){
    struct task **cur;
    struct task *prev = NULL;

    for(cur = &queue_head; *cur != NULL; cur = &(*cur)->next_ready){
        if(*cur == tsk){
            *cur = tsk->next_ready;
            tsk->next_ready = NULL;
            if(queue_tail == tsk){
                queue_tail = prev;
            }
            if(tsk == current_task || tsk == next_task){
                pick_task();
            }
            break;
        }
        prev = *cur;
    }
}


void scheduler(void) {   // pick next task from queue and add current task to tail
    struct task *tsk;
    if(queue_head != NULL && queue_head != queue_tail) {


        tsk = queue_head;
        queue_head = queue_head->next_ready;

        tsk->next_ready = NULL;
        queue_tail->next_ready = tsk;
        queue_tail = tsk;
        pick_task();

    }
}

void block_task(struct task *tsk){
    dequeue_task(tsk);
}

void unblock_task(struct task *tsk){
    enqueue_task(tsk);
}

static void printQ(void){
    kprintf("queue:");
    for(struct task *cur = queue_head; cur != NULL; cur = cur->next_ready){
        kprintf(" %d", cur->pid);
    }
    kprintf("  ");
}

