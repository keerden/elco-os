#ifndef _TASK_H
#define _TASK_H
#include "i386/i386.h"


struct task {
    //arch defined
    intr_stack_t *context;
    phys_bytes page_dir;  
    void* stack_space;
    size_t stack_size;

    int pid;

       
};


void multitasking_init(void);


struct task *create_user_task(void * entry_point);
struct task *create_kernel_task(void * entry_point, size_t stack_size);
int destroy_task(struct task *task);

void scheduler(void);

void pick_task(struct task *next);

extern void save(void);
extern void resume(void);





#endif