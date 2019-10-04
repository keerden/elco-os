#ifndef __KMALLOC
#define __KMALLOC

#include <kstdint.h>
#include <kstddef.h>



  
typedef unsigned int binmap_t;

/***
 * Chunk definition
 */

struct kmalloc_chunk {
    size_t prev_foot;
    size_t header;
    struct kmalloc_chunk*   next;
    struct kmalloc_chunk*   prev;
};


struct kmalloc_tree_chunk {
    size_t prev_foot;
    size_t header;
    struct kmalloc_tree_chunk*   next;
    struct kmalloc_tree_chunk*   prev;

    struct kmalloc_tree_chunk*   left;
    struct kmalloc_tree_chunk*   right;
    struct kmalloc_tree_chunk*   parent;
    binmap_t                     index;
};

typedef struct kmalloc_chunk kmchunk;
typedef struct kmalloc_chunk* kmchunk_ptr;
typedef struct kmalloc_tree_chunk ktchunk;
typedef struct kmalloc_tree_chunk* ktchunk_ptr;




/******* size_t constants ********/
#define SIZE_T_ZERO         ((size_t)0)
#define SIZE_T_ONE          ((size_t)1)
#define SIZE_T_TWO          ((size_t)2)
#define SIZE_T_FOUR         ((size_t)4)
#define SIZE_T_SIZE         (sizeof(size_t))
#define TWO_SIZE_T_SIZES    (SIZE_T_SIZE<<1)
#define FOUR_SIZE_T_SIZES   (SIZE_T_SIZE<<2)
#define SIX_SIZE_T_SIZES    (FOUR_SIZE_T_SIZES+TWO_SIZE_T_SIZES)
#define MAX_SIZE_T          (~(size_t)0)
#define HALF_MAX_SIZE_T     (MAX_SIZE_T / 2U)


/****** chunks *******/

#define PINUSE      (0x1U)
#define CINUSE      (0x2U)
#define FLAGMASK    (0x00000007U)

#define CHUNK_OVERHEAD        (SIZE_T_SIZE)
#define CHUNK_ALIGN_ON        (8U)
#define CHUNK_ALIGN_MASK      (CHUNK_ALIGN_ON - SIZE_T_ONE)
#define MINCHUNKSIZE          (16U)
#define DUMMYSIZE             (CHUNK_ALIGN_ON)

#define IS_INUSE(chunk)             ((chunk)->header & CINUSE)
#define PREV_INUSE(chunk)           ((chunk)->header & PINUSE)
#define GETCHUNKSIZE(c)             (((c)->header) & ~FLAGMASK)
#define CHUNK_PAYLOAD(chunk)        ((void *) ((uint8_t *) chunk + offsetof(struct kmalloc_chunk, next)))
#define CHUNK_PAYLOAD_SIZE(chunk)   (GETCHUNKSIZE(chunk) - CHUNK_OVERHEAD)
#define PAYLOAD_CHUNK(ptr)          ((kmchunk_ptr) ((uint8_t *) ptr - offsetof(struct kmalloc_chunk, next)))
#define CHUNKALIGN(x)               ((kmchunk_ptr) ((uintptr_t) ((uintptr_t)(x) + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK))
#define CHUNKSIZEALIGN(x)           (((x) + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK)
#define CHUNKFLOOR(x)               ((x) & ~CHUNK_ALIGN_MASK)
#define REQUEST2SIZE(req)           ((((req) + CHUNK_OVERHEAD) < MINCHUNKSIZE)? MINCHUNKSIZE : CHUNKSIZEALIGN((req) + CHUNK_OVERHEAD))
#define REQUESTOVERFLOW(req)        (REQUEST2SIZE(req) < (req))
#define NEXTCHUNK(ptr)              ((kmchunk_ptr) ((uint8_t*)ptr + GETCHUNKSIZE(ptr)))
#define PREVCHUNK(ptr)              ((kmchunk_ptr) ((uint8_t*)ptr - (ptr)->prev_foot))
#define CHUNKOFFSET(ptr, off)       ((kmchunk_ptr) ((uint8_t*)ptr + (off)))
/******* Bin types, widths and sizes ********/
#define NSBINS            (32U)
#define NTBINS            (32U)
#define SBIN_SHIFT        (3U)
#define SBIN_WIDTH        (SIZE_T_ONE << SBIN_SHIFT)
#define TBIN_SHIFT        (8U)
#define MIN_LARGE_SIZE    (SIZE_T_ONE << TBIN_SHIFT)
#define MAX_SMALL_SIZE    (MIN_LARGE_SIZE - SIZE_T_ONE)

/******* bin operations ************/
#define small_index(s)      ((s) >> SBIN_SHIFT)
#define small_index2size(i) ((i) << SBIN_SHIFT)

#define MIN_SMALL_INDEX     (small_index(MINCHUNKSIZE))
#define SMALLEST_BINDEX(bin) ((binmap_t) __builtin_ctz(bin))
#define TBIN_DEPTH(index)   (int) (((index) >> 1) + TBIN_SHIFT - 1)


struct kmalloc_state {
    void *heap_start;
    size_t heap_size;
    size_t initial_heap_size;
    binmap_t    sbinmap;
    binmap_t    tbinmap;
    kmchunk_ptr sbin[NSBINS];
    ktchunk_ptr tbin[NTBINS];
    kmchunk_ptr dVictim;
    size_t      dVictimSize;
    kmchunk_ptr topChunk;
    size_t      topChunkSize;
    int         magic;
};

#define KMALLOC_STATE_MAGIC (0xC0FEFE)
#define KMALLOC_IS_INIT(state) ((state).heap_start != NULL && (state).magic == KMALLOC_STATE_MAGIC)
#define ADDR_AFTER_HEAPSTART(addr, state) ((addr) >= (state).heap_start)
#define ADDR_BEFORE_HEAPEND(addr, state) ((addr) < (void *) ((uint8_t*)(state).heap_start + (state).heap_size))
#define ADDRESS_OK(chunk, state) (ADDR_AFTER_HEAPSTART((void*) (chunk), (state)) && ADDR_BEFORE_HEAPEND((void*) (chunk), (state)))

#define HEAP_SHRINK_TRESHOLD (4096U)
#define HEAP_SIZE_ALIGN (4096U)


void kmalloc_init(void *heap_addr, size_t heap_size);

struct kmalloc_state kmalloc_debug_getstate(void);

#define calc_bin_no(size) ((size < MINCHUNKSIZE)? 0 : ((size - MINCHUNKSIZE) / CHUNK_ALIGN_ON))






#endif // !__KMALLOC

