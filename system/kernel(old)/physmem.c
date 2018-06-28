#include <stdint.h>
#include <stdlib.h>
#include <elco-os/kernel/multiboot.h>
#include <elco-os/kernel/kernel.h>
#include <string.h>
#include "physmem.h"

static size_t physmem_size;             /* size of memory from 0x00 to the end of the last available block*/
static int physmem_block_count;         /* total number of blocks */
static int physmem_block_used;          /* number of blocks in use */
static uint32_t* physmem_map;           /* pointer to bitmap */
static size_t physmem_mapsize;          /* size of bitmap */


/*********************************************************
* physmem_mapset
**********************************************************
* Description:
*     marks a block as available in the memory map
* 
* Inputs:
*     blocknumber to set
*********************************************************/

inline static void physmem_mapset(int block) {
    physmem_map[block / 32] |= (1 << (block % 32));
}

/*********************************************************
* physmem_mapclear
**********************************************************
* Description:
*     marks a block as unavailable in the memory map
* 
* Inputs:
*     blocknumber to clear
*********************************************************/
inline static void physmem_mapclear(int block) {
    physmem_map[block / 32] &= ~(1 << (block % 32));
}


//! finds first free frame in the bit array and returns its index
int physmem_first_free () {
    int maxoffset = physmem_block_count /32;
	//! find the first free bit
	for (int i=0; i< maxoffset; i++)
		if (physmem_map[i])
			for (int j=0; j<32; j++) {				//! test each bit in the dword

				int bit = 1 << j;
				if (physmem_map[i] & bit)
					return i*32+j;
			}
    int rem = physmem_block_count % 32;
    if(rem) {
        if (physmem_map[maxoffset])
			for (int j=0; j<rem; j++) {				//! test each bit in the dword

				int bit = 1 << j;
				if (physmem_map[maxoffset] & bit)
					return maxoffset*32+j;
			}      
    }
	return -1;
}

//! finds first free "size" number of frames and returns its index
/*int mmap_first_free_s (size_t size) {
    
	if (size==0)
		return -1;

	if (size==1)
		return mmap_first_free ();

    int maxoffset = physmem_block_count /32;

	for (int i=0; i<maxoffset; i++)
		if (physmem_map[i])
			for (int j=0; j<32; j++) {	//! test each bit in the dword

				int bit = 1<<j;
				if (physmem_map[i] & bit) {

					int startingBit = i*32;
					startingBit+=bit;		//get the free bit in the dword at index i

					uint32_t free=0; //loop through each bit to see if its enough space
					for (uint32_t count=0; count<=size;count++) {

						if (! mmap_test (startingBit+count) )
							free++;	// this bit is clear (free frame)

						if (free==size)
							return i*4*8+j; //free count==size needed; return index
					}
				}
			}

	return -1;
}
*/



/*********************************************************
* physmem_floor
**********************************************************
* Description:
*   Rounds an address to the nearest page boundary
*   that lays before or at the given address
*   
* Inputs:
*   physical_addr addr:     The address to round
*
* Returns:
*   a physical_addr holding the rounded address       
*********************************************************/

inline static physical_addr physmem_floor(physical_addr addr) {
    return addr & ~PHYSMEM_BLOCK_SIZE_MASK;
}

/*********************************************************
* physmem_ceil
**********************************************************
* Description:
*   Rounds an address to the nearest page boundary
*   that lays after or at the given address
*   
* Inputs:
*   physical_addr addr:     The address to round
*
* Returns:
*   a physical_addr holding the rounded address       
*********************************************************/

inline static physical_addr physmem_ceil(physical_addr addr) {
    if(addr & PHYSMEM_BLOCK_SIZE_MASK)
        return physmem_floor(addr) + PHYSMEM_BLOCK_SIZE; 
    else
        return physmem_floor(addr);
}

void physmem_init(multiboot_info_t *bootinfo,  physical_addr krnl_strt, physical_addr krnl_end) 
{
   physical_addr map_location = krnl_end;
   size_t estimated_map_size;
   physical_addr last_segment;
   uint32_t mmap_length;
   uint32_t mmap_addr;
   multiboot_memory_map_t *mmap;
   
   // validate bootinfo map
    if(!(bootinfo->flags & MULTIBOOT_INFO_MEM_MAP && bootinfo->flags & MULTIBOOT_INFO_MEMORY))
    {
        kerror("No memory information specified by bootloader\n");
        abort();
        return;
    }
    mmap_length = bootinfo->mmap_length;
    mmap_addr = bootinfo->mmap_addr;

   // determine if memmap overlaps with the bootinfo and repace it if needed
    estimated_map_size = ((bootinfo->mem_upper + 1048) / 4) / PHYSMEM_BLOCKS_PER_BYTE;
    if(((bootinfo->mem_upper + 1048) / 4) % PHYSMEM_BLOCKS_PER_BYTE)
        estimated_map_size++;
   
    if(map_location <= ((physical_addr) mmap_addr) + mmap_length
        && (physical_addr) mmap_addr <= (map_location + estimated_map_size) ) 
    {
        map_location = physmem_ceil((physical_addr) mmap_addr + mmap_length);
    }
    physmem_map = (uint32_t *) map_location;
       
    // loop through memmap


    last_segment = 0;
    physmem_size = 0;
    physmem_block_used = 0;
    physmem_block_count = 0;
    kdebug("Memory map:\n");


    mmap = (multiboot_memory_map_t *) (bootinfo->mmap_addr);
    while((uint32_t) mmap < (bootinfo->mmap_addr + bootinfo->mmap_length))
    {
        physical_addr segaddr = mmap->addr_low;
        size_t size;


        // if available section is found, update the memmap
        kdebug ("\tStart: %x %x", mmap->addr_high, mmap->addr_low);
        kdebug ("\tlen: %x %x", mmap->len_high, mmap->len_low);
        kdebug ("\ttype: %s\n", (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)? "Available" : "Reserved");
        if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE){
            segaddr = physmem_ceil(segaddr);
            //unset blocks between last segment and this segment
            physmem_block_count += physmem_unset_region(last_segment, segaddr - last_segment);
            size = physmem_floor(mmap->len_low);
            physmem_block_count += physmem_set_region(segaddr, size);
            physmem_size = mmap->addr_low + mmap->len_low; 
            last_segment =  segaddr + size;
        }
        mmap = (multiboot_memory_map_t *) ( (unsigned int) mmap + mmap->size + sizeof(mmap->size) );
    }

    physmem_block_used = (physmem_block_used + physmem_block_count) / 2;
    physmem_mapsize = 1 + (physmem_block_count - 1) / PHYSMEM_BLOCKS_PER_BYTE;


    // reserve the kernel parts


    kdebug ("memmap size: %u bytes. %d blocks %d used\n", physmem_mapsize, physmem_block_count,physmem_block_used);

    int blocks = physmem_unset_region(krnl_strt, physmem_ceil(krnl_end - krnl_strt + physmem_mapsize));
    kdebug ("kernel size: %u bytes. %d blocks  %p\n", physmem_ceil(krnl_end - krnl_strt + physmem_mapsize), blocks, krnl_end);

    kdebug ("memmap size: %u bytes. %d blocks %d used\n", physmem_mapsize, physmem_block_count,physmem_block_used);
}


void* kmalloc() {

    int block =  physmem_first_free ();

    if(block < 0)
        return NULL;

    physmem_mapclear(block);
    physmem_block_used++;
    return (void*) (block * PHYSMEM_BLOCK_SIZE);
}


void kfree(void* p) {

    physical_addr addr = (physical_addr)p;
	int block = addr / PHYSMEM_BLOCK_SIZE;

	physmem_mapset (block);

	physmem_block_used--;
}

/*********************************************************
* physmem_set_region
**********************************************************
* Description:
*   Marks a memory region as available.
*   
* Inputs:
*   physical_addr base:     
*       The address of the start of the region.
*       This address should be alligned to a page boundary
*       If not, it wil be rounded to the next boundary
*
*   size_t size:     
*       The size of the region.  
*       This size should be a multiple of the page size. 
*       If not it will be rounded down.
*
* Returns:
*   An int holding the amount of blocks that are set      
*********************************************************/

int physmem_set_region (physical_addr base, size_t size) {
    if(size == 0)
        return 0;

	int align = physmem_ceil(base) / PHYSMEM_BLOCK_SIZE;
	int blocks = physmem_floor(size) / PHYSMEM_BLOCK_SIZE;

	for (int i = 0; i < blocks; i++) {
		physmem_mapset (align + i);
	}

    physmem_block_used -= blocks;
    return blocks;
}

/*********************************************************
* physmem_unset_region
**********************************************************
* Description:
*   Marks a memory region as unavailable.
*   
* Inputs:
*   physical_addr base:     
*       The address of the start of the region.
*       This address should be alligned to a page boundary
*       If not, it wil be rounded to the next boundary
*
*   size_t size:     
*       The size of the region.  
*       This size should be a multiple of the page size. 
*       If not it will be rounded down.
*
* Returns:
*   An int holding the amount of blocks that are unset      
*********************************************************/

int physmem_unset_region (physical_addr base, size_t size) {
    if(size == 0)
        return 0;

	int align = physmem_ceil(base) / PHYSMEM_BLOCK_SIZE;
	int blocks = physmem_floor(size) / PHYSMEM_BLOCK_SIZE;

	for (int i = 0; i < blocks; i++) {
		physmem_mapclear (align + i);
	}

    physmem_block_used += blocks;

    return blocks;
}   


