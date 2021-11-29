#ifndef __ELCO_OS_KERNEL_TYPE_H
#define __ELCO_OS_KERNEL_TYPE_H

#include <kstdint.h>
#include <elco-os/type.h>
#include <elco-os/config.h>


#define MMAP_TYPE_FREE      0x0001u
#define MMAP_TYPE_KERNEL    UINT32_MAX

struct mem_entry{
    phys_bytes  address;
    phys_bytes  size;
    uint32_t    type;
};


/* This is used to store kernel-specific global info. */
struct kinfo {

    phys_bytes bootimg_start;       /* location of boot image in physical memory*/
    phys_bytes bootimg_size;        /* size of boot image */
    phys_bytes kernel_start_phys;   /* physical addres of kernel */
    phys_bytes kernel_end_phys;     /* physical addres where kernel ends */
    vir_bytes kernel_start_vir;     /* virtual addres of kernel */
    vir_bytes kernel_end_vir;       /* virtual addres where kernel ends */

    char commandline[128];          /* kernel command line parameters */
    struct mem_entry mmap[MAX_MMAP_SIZE];
    int mmap_size;

};

typedef struct message{
    uint8_t contents[MESSAGE_SIZE];
} message_t;


typedef uint16_t message_flags_t;
typedef uint16_t task_flags_t;

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

    /*** messaging ***/
    message_t message_buffer;
    message_t *message_ptr;
    message_flags_t message_flags;
    struct task *message_send_to;
    int message_rec_pid;
    struct task *message_q_head;
    struct task *message_q_next;


    int32_t pid;
    task_flags_t flags;
       
} task_t;





#endif