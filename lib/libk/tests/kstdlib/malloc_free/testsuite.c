#include "testsuite.h"
#include "kstdlib/malloc_free/kmalloc.h"
#include "kstdlib/malloc_free/kmalloc_util.h"
#include <stdio.h>

#define REPORT(str, addr, heap)                                                                       \
    {                                                                                                 \
        printf("line: %d, Error at chunk: %08x -- %s\n", __LINE__, (int)(addr) - (int)(heap), (str)); \
        return 1;                                                                                     \
    }

static int check_chunk(uint8_t *heap, size_t heapsize, kmchunk_ptr chunk);
static int check_tchunk(uint8_t *heap, size_t heapsize, ktchunk_ptr chunk, binmap_t index);

/***
 * int check_heap_integrity(uint8_t *heap, size_t heapsize)
 * 
 * Walks through the heap and checks if all chunks are consistent
 * Free chunks should never follow another free chunk
 * 
 * returns 0 when no errors where found
 ***/

int check_heap_integrity(void *heap, size_t heapsize)
{

    kmchunk_ptr chunk = (kmchunk_ptr)heap;
    int prev_inuse = 1; //first chunk should always indicate PINUSE

    size_t prev_size, size;

    while (chunk->header != 0)
    {

        size = GETCHUNKSIZE(chunk);
        if (((size_t)chunk + size) > ((size_t)heap + heapsize))
            REPORT("Chunksize overflows heap", chunk, heap);
        if (size < MINCHUNKSIZE)
            REPORT("Chunksize too small", chunk, heap);
        if (size % CHUNK_ALIGN_ON)
            REPORT("Chunksize not aligned", chunk, heap);

        if (prev_inuse)
        {
            if (!(chunk->header & PINUSE))
                REPORT("PINUSE not set", chunk, heap);
        }
        else
        {
            if ((chunk->header & PINUSE))
                REPORT("PINUSE is set", chunk, heap);
            if ((chunk->prev_foot != prev_size))
                REPORT("prev_foot doesn't match size of previous chunk", chunk, heap);
            if (!(chunk->header & CINUSE))
                REPORT("A free chunk should never follow another free chunk", chunk, heap);
        }

        prev_size = size;
        prev_inuse = chunk->header & CINUSE;
        chunk = (kmchunk_ptr)((size_t)chunk + size);
    }

    if (!prev_inuse && (chunk->prev_foot != prev_size))
        REPORT("prev_foot of dummy chunk doesn't match size of previous chunk", chunk, heap);
    if (((size_t)chunk + 8) > ((size_t)heap + heapsize))
        REPORT("dummy chunk overflows heap", chunk, heap);
    if (((size_t)chunk + 8) < ((size_t)heap + heapsize))
        REPORT("dummy chunk not placed at end of heap", chunk, heap);
    return 0;
}

/***
 * int check_bins(void *heap, size_t heapsize, struct kmalloc_state state)
 * 
 * walks through every etry in the bin and checks if it is a correct chunk
 * 
 * returns 0 when no errors where found
 ***/

int check_bins(void *heap, size_t heapsize, struct kmalloc_state state)
{

    kmchunk_ptr chunk;
    ktchunk_ptr tchunk;
    for (binmap_t i = 0; i < NSBINS; i++)
    {
        if (state.sbinmap & (1U << i))
        {
            if (state.sbin[i] == NULL)
                REPORT("sbinmask set, but no chunk stored", i, 0);

            //walk
            chunk = state.sbin[i];
            do
            {
                if (!check_chunk(heap, heapsize, chunk))
                    REPORT("sbin chunk is not a valid chunk", chunk, heap);
                if (chunk->header & CINUSE)
                    REPORT("sbin chunk is set inuse", chunk, heap);
                chunk = chunk->next;
            } while (chunk != state.sbin[i]);
        }
        else
        {
            if (state.sbin[i] != NULL)
                REPORT("sbinmask unset, but bin is not empty", i, 0);
        }
    }
    for (binmap_t i = 0; i < NTBINS; i++)
    {
        if (state.tbinmap & (1U << i))
        {
            if (state.tbin[i] == NULL)
                REPORT("tbinmask set, but no chunk stored", i, 0);

            //walk the tree and check each node
            tchunk = state.tbin[i];
            do
            {
                //walk to leftmost leave
                while (tchunk->left != NULL || tchunk->right != NULL)
                    tchunk = (tchunk->left) ? tchunk->left : tchunk->right;

                //check
                if (check_tchunk(heap, heapsize, tchunk, i))
                    return 1;

                //go up until we can go right
                while (tchunk->parent != NULL)
                {
                    if (tchunk->parent->right != NULL && tchunk->parent->right != tchunk)
                    {
                        // we can go up and right
                        tchunk = tchunk->parent->right;
                        break;
                    }
                    tchunk = tchunk->parent;
                    if (check_tchunk(heap, heapsize, tchunk, i))
                        return 1;
                }

            } while (tchunk != state.tbin[i]);
        }
        else
        {
            if (state.tbin[i] != NULL)
                REPORT("tbinmask unset, but bin is not empty", i, 0);
        }
    }

    return 0;
}

/***
 * int check_heap_layout(test_chunkinfo expected, struct kmalloc_state kmstate,uint8_t heap, size_t heapsize)
 * 
 * Walks through the heap and checks if all chunks are consistent with given layout
 * kmstate is checked in order to verify if a chunk is a TC or DV
 * also each free chunk (except TC or DV) must be present in a bin list
 * 
 * returns 0 when no errors where found
 ***/

int check_heap_layout(test_chunkinfo *expected,
                      int testsize,
                      struct kmalloc_state kmstate,
                      void *heap,
                      size_t heapsize)
{
    int index;
    binmap_t binindex;
    size_t size, calc_size, shift;
    kmchunk_ptr chunk = (kmchunk_ptr)heap;
    ktchunk_ptr tchunk;
    kmchunk_ptr listchunk, liststart;
    test_chunkinfo test;

    for (index = 0; index < testsize; index++)
    {
        test = expected[index];
        size = GETCHUNKSIZE(chunk);
        if (test.size != size)
            REPORT("Chunksize mismatch", chunk, heap);
        if (((size_t)chunk + size) > ((size_t)heap + heapsize))
            REPORT("Chunk overflows heap", chunk, heap);

        if (test.type == USED)
        {
            if (!(chunk->header & CINUSE))
                REPORT("Chunk should be in use", chunk, heap);
            if (test.payload != NULL && test.payload != CHUNK_PAYLOAD(chunk))
                REPORT("Payload address mismatch", chunk, heap);
        }
        else
        {
            if (chunk->header & CINUSE)
                REPORT("Chunk should not be in use", chunk, heap);

            switch (test.type)
            {
            case FREE:
                //check small or treebin
                if (size <= MAX_SMALL_SIZE)
                {
                    binindex = small_index(size);
                    if (!(kmstate.sbinmap & (SIZE_T_ONE << binindex)))
                        REPORT("Unused small chunk not present in sbinmap", chunk, heap);
                    if (kmstate.sbin[binindex] == NULL)
                        REPORT("Empty sbin", chunk, heap);
                    liststart = listchunk = kmstate.sbin[binindex];
                    while (listchunk != chunk)
                    {
                        listchunk = listchunk->next;
                        if (listchunk == liststart)
                            break;
                    }
                    if (listchunk != chunk)
                        REPORT("Unused small chunk not present in sbin list", chunk, heap);
                }
                else
                {
                    tchunk = (ktchunk_ptr)chunk;
                    binindex = tchunk->index;
                    //check index matches with size
                    if (binindex > 31)
                    {
                        REPORT("Unused tree chunk, index too large", chunk, heap);
                    }
                    else if (binindex == 31)
                    {
                        if (size <= (0xFFFF << TBIN_SHIFT))
                            REPORT("Unused tree chunk, index mismatch, size too small (1)", chunk, heap);
                    }
                    else
                    {
                        shift = (binindex / 2) + TBIN_SHIFT; //index divided by two gives the 'decade'
                        calc_size = 1U << shift;
                        if (binindex % 2) //if index is odd, use the upper half of the 'decade'
                            calc_size += 1U << (shift - 1);

                        if (size < calc_size)
                            REPORT("Unused tree chunk, index mismatch, size too small (2)", chunk, heap);

                        shift = ((binindex + 1) / 2) + TBIN_SHIFT; //index divided by two gives the 'decade'
                        calc_size = 1U << shift;
                        if ((binindex + 1) % 2) //if index is odd, use the upper half of the 'decade'
                            calc_size += 1U << (shift - 1);

                        if (size >= calc_size)
                            REPORT("Unused tree chunk, index mismatch, size too large (2)", chunk, heap);
                    }
                    if (!(kmstate.tbinmap & (SIZE_T_ONE << binindex)))
                        REPORT("Unused tree chunk not present in tbinmap", chunk, heap);
                    if (kmstate.tbin[binindex] == NULL)
                        REPORT("Empty tbin", chunk, heap);
                    //check if in tbin
                    if (!check_intree(tchunk, kmstate.tbin[binindex]))
                        REPORT("Unused tree chunk not present in tbin tree", chunk, heap);
                }
                break;

            case TC:
                if (kmstate.topChunkSize != size)
                    REPORT("Topchunk size mismatch in kmstate", chunk, heap);
                if (kmstate.topChunk != chunk)
                    REPORT("Topchunk pointer mismatch in kmstate", chunk, heap);
                break;
            case DV:
                if (kmstate.dVictimSize != size)
                    REPORT("dVictim size mismatch in kmstate", chunk, heap);
                if (kmstate.dVictim != chunk)
                    REPORT("dVictim pointer mismatch in kmstate", chunk, heap);
                break;
            default:
                REPORT("Unknown type specified in test_chunkinfo", chunk, heap);
                break;
            }
        }
        chunk = (kmchunk_ptr)((size_t)chunk + size);
    }

    return 0;
}

int check_intree(ktchunk_ptr chunk, ktchunk_ptr root)
{
    ktchunk_ptr search = chunk;
    //check if chunk = root
    if (chunk == root)
        return 1;

    if (chunk->parent == NULL)
    {
        //chunk is not root, so this means it must be in a list of at least 2 chunk
        if (chunk->next == chunk)
            return 0; //list is empty
        search = chunk->next;
        while (search->parent == NULL)
        {
            if (search == root)
                return 1;
            search = search->next;
            if (search == chunk)
                return 0; //we walked through the whole list without result
        }
    }

    //follow the path and check the root
    while (search->parent != NULL)
    {
        search = search->parent;
    }
    return (search == root);
}

void debugDump(char *desc, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char *)addr;

    // Output description if given.
    if (desc != NULL)
        printf("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("  %s\n", buff);

            // Output the offset.
            printf("  %08x ", (int)&pc[i]);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
        {
            buff[i % 16] = '.';
        }
        else
        {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        printf("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf("  %s\n", buff);
}

static int check_chunk(uint8_t *heap, size_t heapsize, kmchunk_ptr chunk)
{
    kmchunk_ptr cur = (kmchunk_ptr)heap;
    kmchunk_ptr end = (kmchunk_ptr)(heap + heapsize);

    while (cur < end && cur->header != 0)
    {
        if (cur == chunk)
            return 1;
        cur = NEXTCHUNK(cur);
    }
    return 0;
}
static int check_tchunk(uint8_t *heap, size_t heapsize, ktchunk_ptr chunk, binmap_t index)
{

    ktchunk_ptr cur = chunk;

    do
    {
        if (!check_chunk(heap, heapsize, (kmchunk_ptr)cur))
            REPORT("tbin chunk is not a valid chunk", chunk, heap);
        if (cur->header & CINUSE)
            REPORT("tbin chunk is set inuse", chunk, heap);
        if (cur->index != index)
            REPORT("tbin chunk index mismatch", chunk, heap);
        cur = cur->next;
    } while (cur != chunk);
    return 0;
} 