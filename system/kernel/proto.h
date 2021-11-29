#ifndef _PROTO_H
#define _PROTO_H

#include <kstddef.h>
#include <kstdbool.h>
#include <kstdio.h>


/* memory.c */

int memory_init(void);
int memory_allocate_page(vir_bytes vaddr, int usermode, int writable);
int memory_free_page(vir_bytes vaddr);
int memory_map_addr(vir_bytes vaddr, phys_bytes frame, int usermode, int writable);
int memory_unmap_addr(vir_bytes vaddr);


/* terminal.c */

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_scroll(bool force);
void terminal_print_logo(void);

/* task.c */
extern task_t *current_task;

void multitasking_init(void);


task_t *create_user_task(void * entry_point);
task_t *create_kernel_task(void * entry_point, size_t stack_size, short priority);
int destroy_task(task_t *task);
task_t *find_task(int32_t pid);
void setPreemptible(task_t *tsk, bool preemtible);

void scheduler(void);
void preempt(void);

void enqueue_task(task_t *tsk);
void dequeue_task(task_t *tsk);

void block_task(task_t *tsk);
void unblock_task(task_t *tsk);

extern void save(void);
extern void resume(void);

/* clock.c */
void clock_setup(void);
void sleep_ms(int ms);

/* message.c */
int message_send(task_t *caller, int32_t dest_pid, message_t *message);
int message_receive(task_t *caller, int32_t origin_pid, message_t *message);
int message_send_receive(task_t *caller, int32_t dest_pid, message_t *message);
bool message_deliver(task_t *caller);



#ifndef _KERNEL_DEBUG
#define kdebug(...)
#else
#define kdebug(...) kprintf(__VA_ARGS__)
#endif

void kerror(const char* error);
void kpanic(const char* message);


#endif