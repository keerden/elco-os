#ifndef _TASK_H
#define _TASK_H
#include "i386/i386.h"

#define TASK_FLAGS_PREEMPTIBLE 0x0001u


typedef struct task {
    //arch defined
    intr_stack_t *context;
    phys_bytes page_dir;  
    void* stack_space;
    size_t stack_size;

    /*** clock ***/
    struct task *timer_next;
    uint32_t timer_expire;

    /*** scheduling ***/
    short priority;
    struct task *next_ready;
    uint16_t ticks_left;
    uint16_t quantum;

    int pid;
    uint16_t flags;
       
} task_t;


extern task_t *current_task;

void multitasking_init(void);


task_t *create_user_task(void * entry_point);
task_t *create_kernel_task(void * entry_point, size_t stack_size, short priority);
int destroy_task(task_t *task);

void scheduler(void);
void preempt(void);

void enqueue_task(task_t *tsk);
void dequeue_task(task_t *tsk);

void block_task(task_t *tsk);
void unblock_task(task_t *tsk);

extern void save(void);
extern void resume(void);





#endif