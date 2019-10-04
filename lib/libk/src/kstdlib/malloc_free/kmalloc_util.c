#include <kstdlib.h>
#include "kmalloc.h"
#include "kmalloc_util.h"
#include "kmalloc_tree.h"

#include "../../callback.h"

struct kmalloc_state kmstate;
   
struct kmalloc_state kmalloc_debug_getstate(void)
{
    return kmstate;
}

void kmalloc_init(void *heap_addr, size_t heap_size)
{
    kmchunk_ptr top_chunk;
    size_t top_size, offset;

    if (heap_addr == NULL)
        return;

    top_chunk = CHUNKALIGN(heap_addr);
    offset = (size_t)top_chunk - (size_t)heap_addr;

    if ((offset + MINCHUNKSIZE + DUMMYSIZE) > heap_size)
        return;

    top_size = CHUNKFLOOR(heap_size - offset - DUMMYSIZE);

    top_chunk->header = top_size | PINUSE;
    top_chunk->next = top_chunk->prev = top_chunk;

    //set dummy
    NEXTCHUNK(top_chunk)->prev_foot = top_size;
    NEXTCHUNK(top_chunk)->header = 0;

    kmstate.heap_start = top_chunk;
    kmstate.heap_size = heap_size - offset;
    kmstate.initial_heap_size = kmstate.heap_size;
    kmstate.sbinmap = kmstate.tbinmap = 0;
    for (size_t i = 0; i < NSBINS; i++)
        kmstate.sbin[i] = NULL;
    for (size_t i = 0; i < NTBINS; i++)
        kmstate.tbin[i] = NULL;
    kmstate.dVictim = NULL;
    kmstate.dVictimSize = 0;
    kmstate.topChunk = top_chunk;
    kmstate.topChunkSize = top_size;
    kmstate.magic = KMALLOC_STATE_MAGIC;
}

void merge_free_chunks(kmchunk_ptr first, size_t first_size, kmchunk_ptr next){
    size_t new_size, next_size;

    if (!IS_INUSE(next))
    {
        next_size = GETCHUNKSIZE(next);
        new_size = first_size + next_size;
        first->header = new_size | PINUSE; 
        CHUNKOFFSET(first, new_size)->prev_foot = new_size;
        if (next == kmstate.topChunk)
        {
            if (first == kmstate.dVictim)
            {
                kmstate.dVictim = NULL;
                kmstate.dVictimSize = 0;
            }
            kmstate.topChunk = first;
            kmstate.topChunkSize = new_size;
            NEXTCHUNK(kmstate.topChunk)->prev_foot = new_size;

            shrink_top();
        }
        else if (next == kmstate.dVictim)
        {
            kmstate.dVictim = first;
            kmstate.dVictimSize = new_size;
        }
        else
        {
            unbin_chunk(next);

            if (first == kmstate.dVictim)
            {
                kmstate.dVictim = first;
                kmstate.dVictimSize = new_size;
            }
            else
            { //add consolidated chunk to bin
                bin_chunk(first);
            }
        }
    }
    else
    {
        first->header = first_size | PINUSE;
        next->prev_foot = first_size;
        next->header &= ~PINUSE;
        if (first == kmstate.dVictim)
        {
            kmstate.dVictim = first;
            kmstate.dVictimSize = first_size;
        }
        else
        { //add consolidated chunk to sbin
            bin_chunk(first);
        }
    }
}

int try_expand_chunk(kmchunk_ptr chunk, size_t new_size)
{
    int result = -1;
    size_t diff, free_size;
    kmchunk_ptr new, next;

    if(!IS_INUSE(chunk) || new_size < GETCHUNKSIZE(chunk))
        return -1;

    diff = new_size - GETCHUNKSIZE(chunk);
    next = NEXTCHUNK(chunk);

    if(!IS_INUSE(next) && GETCHUNKSIZE(next) >= diff){
        //use next chunk and expand
        free_size = GETCHUNKSIZE(next) - diff;
        
        if(free_size > MINCHUNKSIZE) {  //split of remainder
            chunk->header = new_size | CINUSE | PREV_INUSE(chunk);
            
            new = CHUNKOFFSET(next, diff);
            new->header = free_size | PINUSE;
            CHUNKOFFSET(new, free_size)->prev_foot = free_size;

            if(next == kmstate.topChunk){
                    kmstate.topChunk = new;
                    kmstate.topChunkSize = free_size;
            } else if(next == kmstate.dVictim) {
                kmstate.dVictim = new;
                kmstate.dVictimSize = free_size;
            } else {
                unbin_chunk(next);
                bin_chunk(new);
            }
        }else{
            if(free_size > 0){      //remainder is too small to split off, so add it to the size
                diff += free_size;
                new_size += free_size;
            }
            chunk->header = new_size | CINUSE | PREV_INUSE(chunk);
            
            if(next == kmstate.topChunk){
                    kmstate.topChunk = NEXTCHUNK(chunk);
                    kmstate.topChunkSize = 0;
            } else if(next == kmstate.dVictim) {
                kmstate.dVictim = NULL;
                kmstate.dVictimSize = 0;
            } else {
                unbin_chunk(next);
            }               
            next = NEXTCHUNK(chunk);
            if(next->header) 
                next->header |= PINUSE;
        }
        result = 0;
    }
    return result;
}

void *split_top(size_t chunksize, int allow_expand)
{
    kmchunk_ptr chunk = NULL;
    size_t split_size;

    if (kmstate.topChunkSize < chunksize)
    {
        if (!allow_expand || grow_top(chunksize - kmstate.topChunkSize))
            return NULL;
    }

    if (kmstate.topChunkSize - chunksize < MINCHUNKSIZE)
    { //exhaust top
        chunk = kmstate.topChunk;
        chunk->header = chunksize | PINUSE | CINUSE;
        kmstate.topChunk = NEXTCHUNK(chunk);
        kmstate.topChunkSize = 0;
        kmstate.topChunk->header = 0;   //topchunk is now set to the dummy chunk. 
                                        //reset itś header to deal with a incresed heap
    }
    else
    { //split top
        split_size = kmstate.topChunkSize - chunksize;
        chunk = kmstate.topChunk;
        chunk->header = chunksize | PINUSE | CINUSE; //previous chunk cannot be a free chunk, so PINUSE is set
        kmstate.topChunk = CHUNKOFFSET(chunk, chunksize);
        kmstate.topChunk->header = split_size | PINUSE;
        kmstate.topChunkSize = split_size;
        NEXTCHUNK(kmstate.topChunk)->prev_foot = split_size;
    }
    return CHUNK_PAYLOAD(chunk);
}


void shrink_top(void)
{
    size_t decrease, newsize;
    kmchunk_ptr dummy;

    //check if we need to schrink
    //shrink only when top chunk is larger than HEAP_SHRINK_TRESHOLD, but leave a top chunk of at least HEAP_SHRINK_TRESHOLD big
    //dont schrink below initial heap size and align the heap size on boundaries (could be page sizes) defined by HEAP_SIZE_ALIGN.

    if (kmstate.topChunkSize > HEAP_SHRINK_TRESHOLD && kmstate.heap_size >= kmstate.initial_heap_size)
    {
        decrease = kmstate.topChunkSize - HEAP_SHRINK_TRESHOLD;
        newsize = kmstate.heap_size - decrease;
        if(newsize < kmstate.initial_heap_size)
            newsize = kmstate.initial_heap_size;

        //align heapsize
        newsize = (newsize + (HEAP_SIZE_ALIGN - 1)) & ~(HEAP_SIZE_ALIGN - 1);
        decrease = kmstate.heap_size - newsize;

        if(decrease > 0){
            if(decrease > HALF_MAX_SIZE_T)        //prevent signed overflow
                decrease = HALF_MAX_SIZE_T & ~(HEAP_SIZE_ALIGN - 1);


            if(ACTION_SBRK((int) -decrease) != (void *) -1){
                //heap has decreased. change top
                kmstate.heap_size -= decrease;
                kmstate.topChunkSize -= decrease;
                if(kmstate.topChunkSize){
                    kmstate.topChunk->header = kmstate.topChunkSize | PINUSE;
                    dummy = NEXTCHUNK(kmstate.topChunk);
                    dummy->prev_foot = kmstate.topChunkSize;
                    dummy->header = 0;
                } else {
                    kmstate.topChunk->header = 0;
                }
            }

        }
    }
}
int grow_top(size_t increment) {
    kmchunk_ptr dummy;


    increment = (increment + (HEAP_SIZE_ALIGN - 1)) & ~(HEAP_SIZE_ALIGN - 1);

    if(increment > HALF_MAX_SIZE_T)        //prevent signed overflow
        return -1;

    if(ACTION_SBRK((intptr_t) increment) != (void *) -1) {
        kmstate.heap_size += increment;
        kmstate.topChunkSize += increment;
        kmstate.topChunk->header = kmstate.topChunkSize | PINUSE;
        dummy = NEXTCHUNK(kmstate.topChunk);
        dummy->prev_foot = kmstate.topChunkSize;
        dummy->header = 0;

        return 0;
    }

    return -1; 
     
}

void *allocate_dv(size_t chunksize)
{
    kmchunk_ptr chunk = NULL;
    size_t split_size;

    if (kmstate.dVictim == NULL || kmstate.dVictimSize < chunksize)
        return NULL;

    if (kmstate.dVictimSize - chunksize < MINCHUNKSIZE)
    { //exhaust dv
        chunk = kmstate.dVictim;
        chunk->header |= CINUSE;
        NEXTCHUNK(chunk)->header |= PINUSE;
        kmstate.dVictim = NULL;
        kmstate.dVictimSize = 0;
    }
    else
    { //split dv
        split_size = kmstate.dVictimSize - chunksize;
        chunk = kmstate.dVictim;
        chunk->header = chunksize | PINUSE | CINUSE; //previous chunk cannot be a free chunk, so PINUSE is set
        kmstate.dVictim = CHUNKOFFSET(chunk, chunksize);
        kmstate.dVictim->header = split_size | PINUSE;
        kmstate.dVictimSize = split_size;
        NEXTCHUNK(kmstate.dVictim)->prev_foot = split_size;
    }
    return CHUNK_PAYLOAD(chunk);
}

void replace_dv(kmchunk_ptr new)
{
    if (kmstate.dVictim != NULL)
    {
        bin_chunk(kmstate.dVictim);
    }

    kmstate.dVictim = new;
    kmstate.dVictimSize = GETCHUNKSIZE(new);
}

void bin_chunk(kmchunk_ptr chunk)
{
    size_t index, size = GETCHUNKSIZE(chunk);
    ktchunk_ptr tchunk;
    if (size < MIN_LARGE_SIZE)
    {
        index = small_index(size);
        kmalloc_dllist_add(chunk, &kmstate.sbin[index]);
        kmstate.sbinmap |= (1U << index);
    }
    else
    {
        index = calc_tbin(size);
        tchunk = (ktchunk_ptr)chunk;
        tchunk->index = index;
        kmalloc_tree_insert(tchunk, &kmstate.tbin[index], TBIN_DEPTH(index));
        kmstate.tbinmap |= (1U << index);
    }
}

void unbin_chunk(kmchunk_ptr chunk)
{
    size_t index, size = GETCHUNKSIZE(chunk);
    if (size < MIN_LARGE_SIZE)
    {
        index = small_index(size);
        kmalloc_dllist_remove_intern(chunk, &kmstate.sbin[index]);
        if (kmstate.sbin[index] == NULL)
            kmstate.sbinmap &= ~(SIZE_T_ONE << index);
    }
    else
    {
        index = ((ktchunk_ptr)chunk)->index;
        kmalloc_tree_remove((ktchunk_ptr)chunk, &kmstate.tbin[index]);
        if (kmstate.tbin[index] == NULL)
            kmstate.tbinmap &= ~(SIZE_T_ONE << index);
    }
}

void *split_sbin(binmap_t index, size_t chunksize)
{

    kmchunk_ptr chunk, new;
    size_t split_size;

    chunk = kmalloc_dllist_remove(&kmstate.sbin[index]);

    if (kmstate.sbin[index] == NULL)
        kmstate.sbinmap &= ~(SIZE_T_ONE << index);
    if (chunk == NULL)
        return NULL;

    split_size = GETCHUNKSIZE(chunk) - chunksize;
    chunk->header = chunksize | PINUSE | CINUSE; //previous chunk cannot be a free chunk, so PINUSE is set

    new = CHUNKOFFSET(chunk, chunksize);
    new->header = split_size | PINUSE;
    NEXTCHUNK(new)->prev_foot = split_size;

    replace_dv(new);

    return CHUNK_PAYLOAD(chunk);
}

void *allocate_sbin(binmap_t index)
{

    kmchunk_ptr chunk = kmalloc_dllist_remove(&kmstate.sbin[index]);
    if (kmstate.sbin[index] == NULL)
        kmstate.sbinmap &= ~(SIZE_T_ONE << index);
    if (chunk == NULL)
        return NULL;

    chunk->header |= CINUSE;
    NEXTCHUNK(chunk)->header |= PINUSE;
    return CHUNK_PAYLOAD(chunk);
}

void *allocate_tchunk(ktchunk_ptr chunk, size_t wanted_size)
{
    kmchunk_ptr new;
    size_t splitsize;

    if (chunk == NULL)
        return NULL;

    kmalloc_tree_remove(chunk, &kmstate.tbin[chunk->index]);
    if (!kmstate.tbin[chunk->index])
        kmstate.tbinmap &= ~(1U << chunk->index);

    if (GETCHUNKSIZE(chunk) - wanted_size > MINCHUNKSIZE)
    {
        //split
        splitsize = GETCHUNKSIZE(chunk) - wanted_size;
        new = CHUNKOFFSET(chunk, wanted_size);
        chunk->header = wanted_size | CINUSE | PINUSE; //since chunk was free, prev chunk is always in use
        new->header = splitsize | PINUSE;
        NEXTCHUNK(new)->prev_foot = splitsize;
        replace_dv(new);
    }
    else
    {
        chunk->header |= CINUSE;
        NEXTCHUNK(chunk)->header |= PINUSE;
    }

    return CHUNK_PAYLOAD(chunk);
}

void *allocate_smallest_tbin(size_t wanted_size)
{

    binmap_t tbin = calc_leftbin(0, kmstate.tbinmap);

    ktchunk_ptr chunk = kmalloc_tree_get_smallest(kmstate.tbin[tbin]);

    return allocate_tchunk(chunk, wanted_size);
}

