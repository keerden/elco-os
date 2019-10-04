#include <kstdlib.h>
#include "kmalloc.h"
#include "kmalloc_util.h"

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

    merge_free_chunks(freeptr, freesize, next);
}