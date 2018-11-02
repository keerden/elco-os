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

#endif