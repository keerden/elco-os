#include <libk.h>

#include "kernel.h"

static uint32_t clock_ticks = 0;

static task_t *next_timer_event;

static void timer_callback(void)
{
    ++clock_ticks;

    if(current_task->flags & TASK_FLAGS_PREEMPTIBLE){
        current_task->ticks_left--;
        if(current_task->ticks_left == 0){
            current_task->ticks_left = current_task->quantum;
            preempt();
        }
    }

    if(next_timer_event != NULL && next_timer_event->timer_expire <= clock_ticks){

        while(next_timer_event != NULL && next_timer_event->timer_expire <= clock_ticks){
            task_t* tsk = next_timer_event;
            next_timer_event = tsk->timer_next;
            tsk->timer_next = NULL;
            unblock_task(tsk);
        }

    }

}


void clock_setup(void){
    next_timer_event = NULL;
    arch_timer_init(KERNEL_CLOCK_FREQ, &timer_callback);
}

void sleep_ms(int ms){
    if(ms <= 0)
        return;
    
    task_t **nxtPtr;


    uint32_t ticks = (uint32_t) ((KERNEL_CLOCK_FREQ * ms) + 999) / 1000;
    ticks += clock_ticks;

    current_task->timer_expire = ticks;

    for(nxtPtr = &next_timer_event; *nxtPtr != NULL; nxtPtr = &(*nxtPtr)->timer_next){
        if((*nxtPtr)->timer_expire > ticks)
            break;
    }

    current_task->timer_next = *nxtPtr;
    *nxtPtr = current_task;  
    block_task(current_task);

    // kprintf("sleep(%d)queue:", ms);
    // for(task_t *cur = next_timer_event; cur != NULL; cur = cur->timer_next){
    //     kprintf(" %d->%d", cur->pid, cur->timer_expire);
    // }
    // kprintf("\n");
};