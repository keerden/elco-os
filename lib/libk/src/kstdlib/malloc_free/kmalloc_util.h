#ifndef __KMALLOC_UTIL
#define __KMALLOC_UTIL

#include "kmalloc.h"
#include "../../callback.h"

#define ACTION_SBRK(increment)  __SBRK_CALLBACK(increment)
#define ACTION_ABORT()          __ABORT_CALLBACK()   

extern struct kmalloc_state kmstate;



void *kmalloc_intern(size_t chunksize, int allow_expand);

void merge_free_chunks(kmchunk_ptr first, size_t first_size, kmchunk_ptr next);
int try_expand_chunk(kmchunk_ptr chunk, size_t new_size);



void *split_top(size_t chunksize, int allow_expand);
void shrink_top(void);
int grow_top(size_t increment);

void *allocate_dv(size_t chunksize);
void replace_dv(kmchunk_ptr newdv);

void bin_chunk(kmchunk_ptr chunk);
void unbin_chunk(kmchunk_ptr chunk);

void *split_sbin(binmap_t index, size_t chunksize);
void *allocate_sbin(binmap_t size);
void *allocate_tchunk(ktchunk_ptr chunk, size_t wanted_size);
void *allocate_smallest_tbin(size_t wanted_size);

inline binmap_t calc_tbin(size_t size)
{
    if (size < (1 << TBIN_SHIFT))
        return 0;
    if (size > (0xFFFF << TBIN_SHIFT))
        return 31;

    binmap_t msb = 31U - (binmap_t) __builtin_clz(size);
    //msb * 2, add 1 if size lays in higher half of the decade
    return ((msb - TBIN_SHIFT) << 1) | ((size >> (msb - 1)) & 1);
}

inline binmap_t calc_leftbin(binmap_t index, binmap_t map)
{
    binmap_t mask = -(1U << index); //maskout all right bits
    return (binmap_t) __builtin_ctz(map & mask);
}

#endif