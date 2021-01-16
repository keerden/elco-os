#ifndef _TASK_H
#define _TASK_H
#include "i386/i386.h"



struct task {
    //arch defined
    intr_stack_t *context;
    phys_bytes page_dir;  
    void* stack_space;
    size_t stack_size;

    /*** clock ***/
    struct task *timer_next;
    uint32_t timer_expire;

    /*** scheduling ***/
    int priority;
    struct task *next_ready;

    int pid;
       
};

extern struct task *current_task;


void multitasking_init(void);


struct task *create_user_task(void * entry_point);
struct task *create_kernel_task(void * entry_point, size_t stack_size);
int destroy_task(struct task *task);

void scheduler(void);

void pick_task(void);
void enqueue_task(struct task *tsk);
void dequeue_task(struct task *tsk);

void block_task(struct task *tsk);
void unblock_task(struct task *tsk);

extern void save(void);
extern void resume(void);





#endif