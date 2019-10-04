#include <kstdlib.h>
#include <kstring.h>
#include "kmalloc.h"
#include "kmalloc_util.h"


void *krealloc( void *ptr, size_t req_size ) {
    kmchunk_ptr next, chunk, new;
    size_t new_size, cur_size, diff, old_capacity;
    void *new_location;
    void *result = NULL;

    if(ptr == NULL)
        return kmalloc(req_size);
    
    if(req_size == 0){
        kfree(ptr);
        return NULL;
    }
    
    //do sanity checks before moving on
    if(!KMALLOC_IS_INIT(kmstate))
        return NULL;

    chunk = PAYLOAD_CHUNK(ptr);

    if (!ADDRESS_OK(chunk, kmstate) || !IS_INUSE(chunk))
    {
        ACTION_ABORT();
        return NULL;
    }

    next = NEXTCHUNK(chunk);

    if (!ADDRESS_OK(next, kmstate)){
        ACTION_ABORT();
        return NULL;
    }

    old_capacity = CHUNK_PAYLOAD_SIZE(chunk);
    //check of we need to grow or schrink
    new_size = REQUEST2SIZE(req_size); 
    cur_size = GETCHUNKSIZE(chunk);

    if(new_size == cur_size) 
        return ptr;

    if(new_size < cur_size){ //split and free
        diff = cur_size - new_size;

        if(diff < MINCHUNKSIZE && IS_INUSE(next))   //remainder is too small
            return ptr;

        chunk->header = new_size | CINUSE | PREV_INUSE(chunk);
        new = NEXTCHUNK(chunk);
        merge_free_chunks(new, diff, next);
        result = ptr;

    } else {
        if(try_expand_chunk(chunk, new_size) == 0){            //try to expand chunk    
            result = ptr;
        } else if((new_location = kmalloc_intern(new_size, 1))) { //try to malloc new chunk and copy
 
            result = kmemcpy(new_location, ptr, old_capacity);
            kfree(ptr);
        }
        //TODO: simulate free before malloc
    }

    return result;
}
