#include <kstdlib.h>
#include "kmalloc.h"
#include "kmalloc_util.h"
#include "kmalloc_tree.h"



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
    void *result = NULL;

    if (!KMALLOC_IS_INIT(kmstate))
        return NULL;
    if (REQUESTOVERFLOW(size))
        return NULL;

    chunksize = REQUEST2SIZE(size);
    result = kmalloc_intern(chunksize, 1);

    return result;    
}

void *kmalloc_intern(size_t chunksize, int allow_expand)
{
    binmap_t index, dvindex, bits;
    void *result = NULL;
    ktchunk_ptr chunk;

    if (chunksize < MIN_LARGE_SIZE)
    { // use small bins
        index = small_index(chunksize);
        bits = (kmstate.sbinmap >> index);
        
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
            result = split_top(chunksize, allow_expand);
        }
    }

    return result;
}