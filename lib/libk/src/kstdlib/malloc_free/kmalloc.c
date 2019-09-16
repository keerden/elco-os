#include <kstdlib.h>
#include "kmalloc.h"
#include "kmalloc_util.h"
#include "../../callback.h"

static struct kmalloc_state kmstate;

static void *allocate_sbin(binmap_t size);
static void *split_sbin(binmap_t index, size_t chunksize);
static void *allocate_dv(size_t chunksize);
static void *allocate_tchunk(ktchunk_ptr chunk, size_t wanted_size);
static void *allocate_smallest_tbin(size_t wanted_size);
static void *split_top(size_t chunksize);
static void replace_dv(kmchunk_ptr new);
static void bin_chunk(kmchunk_ptr chunk);
static void unbin_chunk(kmchunk_ptr chunk);

static void shrink_top(void);
static int grow_top(size_t increment);

static inline binmap_t calc_tbin(size_t size);
static inline binmap_t calc_leftbin(binmap_t index, binmap_t map);

#define ACTION_SBRK(increment)  __SBRK_CALLBACK(increment)
#define ACTION_ABORT()            __ABORT_CALLBACK()           



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

/****
 * void* kmalloc (size_t size)
 * 
 * Allocates a block of at least 'size' bytes on heap. 
 * 
 * Input:
 *      size_t size:  Size of block to allocate. Could also be zero
 * 
 * Returns: 
 *      A pointer to the block of memory or NULL if there was a failure
 *****/

void *kmalloc(size_t size)
{
    size_t chunksize;
    binmap_t index, dvindex, bits;
    void *result = NULL;
    ktchunk_ptr chunk;

    if (!KMALLOC_IS_INIT(kmstate))
        return NULL;
    if (REQUESTOVERFLOW(size))
        return NULL;

    chunksize = REQUEST2SIZE(size);

    if (chunksize < MIN_LARGE_SIZE)
    { // use small bins
        index = small_index(chunksize);
        bits = (kmstate.sbinmap >> index);
        ;
        if (bits & 0x3U)
        {                       //check if there could be a remainderless fit
            index += ~bits & 1; //increase index if exact matching bin is empty
            result = allocate_sbin(index);
        }
        else if (chunksize > kmstate.dVictimSize)
        {
            //find a suitable bin
            index += 2;
            bits >>= 2;
            if (bits)
            {
                index += calc_leftbin(0, bits);
                result = split_sbin(index, chunksize);
            }
            else if (kmstate.tbinmap)
            {
                result = allocate_smallest_tbin(chunksize);
            }
        }
    }
    else
    { //use tree bins
        index = calc_tbin(chunksize);
        chunk = kmalloc_tree_get_best_fit(chunksize, kmstate.tbin[index], TBIN_DEPTH(index));

        if (chunk != NULL)
        { //if we found a fitting chunk
            //if it fits better, use chunk, else dv wil be used later on
            if (chunksize > kmstate.dVictimSize || GETCHUNKSIZE(chunk) <= kmstate.dVictimSize)
                result = allocate_tchunk(chunk, chunksize);
        }
        else
        {
            //find index of next smallest bin
            index++;
            bits = kmstate.tbinmap >> index;
            if (bits)
            {
                index += calc_leftbin(0, bits);
                dvindex = calc_tbin(kmstate.dVictimSize);
                if (dvindex >= index || chunksize > kmstate.dVictimSize)
                {
                    //if the dv is not necessary the best fit
                    chunk = kmalloc_tree_get_smallest(kmstate.tbin[index]);
                    if (chunksize > kmstate.dVictimSize || GETCHUNKSIZE(chunk) < kmstate.dVictimSize)
                        result = allocate_tchunk(chunk, chunksize);
                }
            }
        }
    }

    if (result == NULL)
    { //we failed to allocate from a bin, or find out that we had to use dv
        if (chunksize <= kmstate.dVictimSize)
        {
            result = allocate_dv(chunksize);
        }
        else
        {
            //get it from the top
            result = split_top(chunksize);
        }
    }

    return result;
}

/****
 * void kfree(void *ptr) 
 * 
 * Returns a block to the free list.
 * 
 * Input:
 *      void *ptr, pointer to allocated block
 * 
 * 
 *****/

void kfree(void *ptr) 
{
    kmchunk_ptr freeptr, prev, next, chunk = PAYLOAD_CHUNK(ptr);

    size_t freesize, csize;

    if(ptr == NULL || !KMALLOC_IS_INIT(kmstate))
        return;

    if (!ADDRESS_OK(chunk, kmstate) || !IS_INUSE(chunk))
    {
        ACTION_ABORT();
        return;
    }

    freeptr = chunk;
    freesize = GETCHUNKSIZE(chunk);

    if (!PREV_INUSE(chunk))
    {
        csize = chunk->prev_foot;
        prev = PREVCHUNK(chunk);
        if (!ADDRESS_OK(prev, kmstate) || GETCHUNKSIZE(prev) != csize || IS_INUSE(prev))
        {
            ACTION_ABORT();
            return;
        }
        freeptr = prev;
        freesize += csize;

        if (kmstate.dVictim != prev)
        { //if not dv, remove prev from bin
            unbin_chunk(prev);
        }
    }
    next = NEXTCHUNK(chunk);

    if (!ADDRESS_OK(next, kmstate)){
        ACTION_ABORT();
        return;
    }

    if (!IS_INUSE(next))
    {
        csize = GETCHUNKSIZE(next);
        freesize += csize;
        freeptr->header = freesize | PREV_INUSE(freeptr);
        CHUNKOFFSET(freeptr, freesize)->prev_foot = freesize;
        if (next == kmstate.topChunk)
        {
            if (freeptr == kmstate.dVictim)
            {
                kmstate.dVictim = NULL;
                kmstate.dVictimSize = 0;
            }
            kmstate.topChunk = freeptr;
            kmstate.topChunkSize = freesize;
            NEXTCHUNK(kmstate.topChunk)->prev_foot = freesize;

            shrink_top();
        }
        else if (next == kmstate.dVictim)
        {
            kmstate.dVictim = freeptr;
            kmstate.dVictimSize = freesize;
        }
        else
        {
            unbin_chunk(next);

            if (freeptr == kmstate.dVictim)
            {
                kmstate.dVictim = freeptr;
                kmstate.dVictimSize = freesize;
            }
            else
            { //add consolidated chunk to bin
                bin_chunk(freeptr);
            }
        }
    }
    else
    {
        freeptr->header = freesize | PREV_INUSE(freeptr);
        next = CHUNKOFFSET(freeptr, freesize);
        next->prev_foot = freesize;
        next->header &= ~PINUSE;
        if (freeptr == kmstate.dVictim)
        {
            kmstate.dVictim = freeptr;
            kmstate.dVictimSize = freesize;
        }
        else
        { //add consolidated chunk to sbin
            bin_chunk(freeptr);
        }
    }
}

void *krealloc( void *ptr, size_t new_size ) {
    /*
        not implemented yet.

        Basic outline will be:
            -   if ptr = NULL malloc
            -   if new_size = 0 free
            -   if newsize is equal, return ptr
            -   else if newsize is smaller, 
                    split chunk and free it.
            -   else, the size is larger
            -       if next chunk is free and large enough, expand
            -       else malloc, copy free? 
            -
    */
    return NULL;
}
	


static void *allocate_sbin(binmap_t index)
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

static void *split_sbin(binmap_t index, size_t chunksize)
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
    ;
}

static void *allocate_dv(size_t chunksize)
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

static void replace_dv(kmchunk_ptr new)
{
    if (kmstate.dVictim != NULL)
    {
        bin_chunk(kmstate.dVictim);
    }

    kmstate.dVictim = new;
    kmstate.dVictimSize = GETCHUNKSIZE(new);
}

static void bin_chunk(kmchunk_ptr chunk)
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

static void unbin_chunk(kmchunk_ptr chunk)
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

static void *allocate_tchunk(ktchunk_ptr chunk, size_t wanted_size)
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

static void *allocate_smallest_tbin(size_t wanted_size)
{

    binmap_t tbin = calc_leftbin(0, kmstate.tbinmap);

    ktchunk_ptr chunk = kmalloc_tree_get_smallest(kmstate.tbin[tbin]);

    return allocate_tchunk(chunk, wanted_size);
}

static void *split_top(size_t chunksize)
{
    kmchunk_ptr chunk = NULL;
    size_t split_size;

    if (kmstate.topChunkSize < chunksize)
    {
        if (grow_top(chunksize - kmstate.topChunkSize))
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

static inline binmap_t calc_tbin(size_t size)
{
    if (size < (1 << TBIN_SHIFT))
        return 0;
    if (size > (0xFFFF << TBIN_SHIFT))
        return 31;

    binmap_t msb = 31U - (binmap_t) __builtin_clz(size);
    //msb * 2, add 1 if size lays in higher half of the decade
    return ((msb - TBIN_SHIFT) << 1) | ((size >> (msb - 1)) & 1);
}

static inline binmap_t calc_leftbin(binmap_t index, binmap_t map)
{
    binmap_t mask = -(1U << index); //maskout all right bits
    return (binmap_t) __builtin_ctz(map & mask);
}

static void shrink_top(void)
{
    size_t decrease, newsize;
    kmchunk_ptr dummy;

    //check if we need to schrink
    //shrink only when top chunk is larger than HEAP_SHRINK_TRESHOLD, but leave a top chunk of at least HEAP_SHRINK_TRESHOLD big
    //dont schrink below initial heap size and align the heap size on boundaries (could be page sizes) defined by HEAP_SIZE_ALIGN.

    if (kmstate.topChunkSize > HEAP_SHRINK_TRESHOLD && kmstate.heap_size > kmstate.initial_heap_size)
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
static int grow_top(size_t increment) {
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