#include "kernel.h"
#include "task.h"

#include <libk.h>
#include <kstring.h>
#include <kstdlib.h>

#define TASK_COUNT 10

static void printQ(short prio);
static void idle(void);

int nested_intr;

task_t ptable[TASK_COUNT];
task_t *idle_task;
task_t *next_task;
task_t *current_task;

static bool reschedule_needed;
static int pid_count;
static task_t *queue_head[LOWEST_TASK_PRIORITY + 1];
static task_t *queue_tail[LOWEST_TASK_PRIORITY + 1];


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

    for(short i = 0; i <= LOWEST_TASK_PRIORITY ; ++i){
        queue_head[i] = queue_tail[i] = NULL;
    }

    idle_task = create_kernel_task(idle, 128, LOWEST_TASK_PRIORITY); //TODO: change stack size
    idle_task->flags &= ~TASK_FLAGS_PREEMPTIBLE; //no need to preempt idle on clock tick
    reschedule_needed = true;
}


task_t *create_kernel_task(void * entry_point, size_t stack_size, short priority)
{
    task_t *tsk = NULL;
    void *stack;
    vir_bytes sp;
    intr_stack_t *context;

    if(priority < 0 || priority > LOWEST_TASK_PRIORITY)
        return NULL;

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
            tsk->priority = priority;
            tsk->flags = TASK_FLAGS_PREEMPTIBLE;
            tsk->quantum = TASK_DEFAULT_QUANTUM;
            tsk->ticks_left = tsk->quantum;

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

void enqueue_task(task_t *tsk){
    short prio = tsk->priority;
    tsk->next_ready = NULL;

    if(queue_head[prio] == NULL) {
        queue_head[prio] = queue_tail[prio] = tsk;
    } else {
        queue_tail[prio]->next_ready = tsk;
        queue_tail[prio] = tsk;
    }
    reschedule_needed = true;
}
void dequeue_task(task_t *tsk){
    task_t **cur;
    task_t *prev = NULL;
    short prio = tsk->priority;

    for(cur = &queue_head[prio]; *cur != NULL; cur = &(*cur)->next_ready){
        if(*cur == tsk){
            *cur = tsk->next_ready;
            tsk->next_ready = NULL;
            if(queue_tail[prio] == tsk){
                queue_tail[prio] = prev;
            }
            if(tsk == current_task || tsk == next_task){
                reschedule_needed = true;
            }
            break;
        }
        prev = *cur;
    }
}

void scheduler(void) {   // pick next task from queue and add current task to tail
    if(reschedule_needed){
        reschedule_needed = false;
        next_task = idle_task;
        for(short i = 0; i <= LOWEST_TASK_PRIORITY ; ++i){
            if(queue_head[i] != NULL){
                next_task = queue_head[i];
                break;
            }
        }
    }
}
    

void preempt(void){
    dequeue_task(current_task);
    enqueue_task(current_task);
}

void block_task(task_t *tsk){
    dequeue_task(tsk);
}

void unblock_task(task_t *tsk){
    enqueue_task(tsk);
}

static void printQ(short prio){
    if(prio <0 || prio > LOWEST_TASK_PRIORITY)
        return;

    kprintf("queue:");
    for(task_t *cur = queue_head[prio]; cur != NULL; cur = cur->next_ready){
        kprintf(" %d", cur->pid);
    }
    kprintf("  ");
}

static void idle(void){ //idle task
    int a = 0;
    while(1){
        arch_display_number(a++, 40, 15);
        asm volatile("hlt");
    }

}