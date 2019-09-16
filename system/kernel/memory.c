/*  This file contains the code for memory management and paging. 
 * 
 *  TODO:separate the i386 specific code
 * 
 */

#include "kernel.h"
#include <kstring.h>
#include <libk.h>
#include <kstdlib.h>

#define PTE_MAX 1024u
#define PDE_MAX 1024u

#define PTABLE_FLAG_PRESENT 0x001u
#define PTABLE_FLAG_WRITE 0x002u
#define PTABLE_FLAG_USERMODE 0x004u
#define PTABLE_FLAG_WT 0x008u
#define PTABLE_FLAG_NO_CACHE 0x010u
#define PTABLE_FLAG_ACCESSED 0x020u
#define PTABLE_FLAG_DIRTY 0x040u
#define PTABLE_FLAG_PAT 0x080u
#define PTABLE_FLAG_LARGEPAGE 0x080u
#define PTABLE_FLAG_GLOBAL 0x100u

#define PTABLE_FLAGS_MASK 0xFFFu
#define PTABLE_DIR_MASK 0xFFC00000u
#define PTABLE_TABLE_MASK 0x003FF000u
#define PTABLE_CR3_MASK 0xFFFFF000u

#define PTABLE_PDE_NUM(addr) ((addr) >> 22)
#define PTABLE_PTE_NUM(addr) (((addr) & ~PTABLE_DIR_MASK) >> 12)

struct page_table
{
    uint32_t entry[PTE_MAX];
};

struct page_directory
{
    uint32_t ptable[PDE_MAX];
};

static struct page_directory *kernel_pde;
static struct page_table *kernel_page_tables;
static struct page_table *pstack_page_table;
static phys_bytes pstack_pointer;
static size_t kernel_heap_size;
static vir_bytes kernel_heap_addr;

static int set_page(vir_bytes vaddr, phys_bytes frame, uint32_t flags);
static int get_page(vir_bytes vaddr, phys_bytes *frame, uint32_t *flags);
static void set_page_table(vir_bytes vaddr, phys_bytes table, uint32_t flags, int clear);
static void set_page_directory(phys_bytes directory);
static inline int page_is_mapped(vir_bytes addr);
static inline int ptable_is_set(vir_bytes addr);

static int pstack_push(phys_bytes frame);
static int pstack_pop(phys_bytes *frame);

static void *heap_callback(intptr_t difference);

/****************
 * set_page
 **************** 
 * 
 * Sets a page inside a page table and invalidate page in TLB
 * 
 * Inputs:
 *      - vaddr: virtual address of the page to set
 *      - frame: physiscal addres to map to
 *      - flags: flags containing settings
 * 
 * Returns:
 *  0 on success, 1 when the corresponding page table doesn't exist
 *
 ***************/

static int set_page(vir_bytes vaddr, phys_bytes frame, uint32_t flags)
{
    vir_clicks page;
    vir_clicks table = PTABLE_PDE_NUM(vaddr);

    if (!((uint32_t)kernel_pde->ptable[table] & PTABLE_FLAG_PRESENT) ||
        ((uint32_t)kernel_pde->ptable[table] & PTABLE_FLAG_LARGEPAGE))
    {
        return 1;
    }
    page = PTABLE_PTE_NUM(vaddr);
    flags &= PTABLE_FLAGS_MASK;
    frame &= ~PTABLE_FLAGS_MASK;
    kernel_page_tables[table].entry[page] = (uint32_t)frame | flags;

    __asm__ __volatile__("invlpg (%0)"
                         :
                         : "r"(vaddr));
    return 0;
}

/****************
 * get_page
 **************** 
 * 
 * Retrieves an entry from the page table
 * 
 * Inputs:
 *      - vaddr: virtual address of the page to retrieve
 * 
 * Outputs
 *      - frame: physiscal addres the page is mapped to
 *      - flags: flags containing settings
 * 
 * Returns:
 *  0 on success, 1 when the corresponding page table doesn't exist
 *
 ***************/

static int get_page(vir_bytes vaddr, phys_bytes *frame, uint32_t *flags)
{
    vir_clicks page;
    vir_clicks table = PTABLE_PDE_NUM(vaddr);

    if (!(kernel_pde->ptable[table] & PTABLE_FLAG_PRESENT) ||
        (kernel_pde->ptable[table] & PTABLE_FLAG_LARGEPAGE))
    {
        return 1;
    }
    page = PTABLE_PTE_NUM(vaddr);

    *flags = kernel_page_tables[table].entry[page] & PTABLE_FLAGS_MASK;
    *frame = kernel_page_tables[table].entry[page] & ~PTABLE_FLAGS_MASK;

    return 0;
}

/****************
 * set_page_table
 **************** 
 * 
 * Sets a page table inside the page directory and invalidate TLB
 * 
 * Inputs:
 *      - vaddr: starting virtual address of the page table to set
 *      - table: physiscal addres of the page table
 *      - flags: flags containing settings
 *      - clear: if set, all the entries are reset.
 * 
 ***************/
#pragma GCC push_options
#pragma GCC optimize("O0")

static void set_page_table(vir_bytes vsaddr, phys_bytes table, uint32_t flags, int clear)
{

    vir_clicks vclick = PTABLE_PDE_NUM(vsaddr);
    flags &= PTABLE_FLAGS_MASK;
    table &= ~PTABLE_FLAGS_MASK;
    kernel_pde->ptable[vclick] = (table | flags);

    __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %%cr3"
                         :
                         :
                         : "%eax", "memory");
    if (clear)
    {
        for (unsigned int i = 0; i < PTE_MAX; i++)
        {
            kernel_page_tables[vclick].entry[i] = 0;
        }

        __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %%cr3"
                             :
                             :
                             : "%eax", "memory");
    }
}
#pragma GCC pop_options
/****************
 * set_page_directory
 **************** 
 * 
 * Points the processor to the given page directory and flushes TLB
 * 
 * Inputs:
 *      - directory: starting virtual address of the page table to set
 * 
 ***************/

static void set_page_directory(phys_bytes directory)
{
    directory &= ~PTABLE_CR3_MASK;
    __asm__ __volatile__("movl %0, %%cr3"
                         :
                         : "r"(directory));
}

/****************
 * page_is_mapped
 **************** 
 * 
 * Checks if virtual address is mapped to physical memory
 * 
 * Inputs:
 *      - vaddr: virtual address to check
 * 
 * Returns:
 *      - TRUE if the address is mapped, FALSE otherwise 
 ***************/

static inline int page_is_mapped(vir_bytes vaddr)
{
    vir_clicks pte;
    vir_clicks pde = PTABLE_PDE_NUM(vaddr);
    if (!((uint32_t)kernel_pde->ptable[pde] & PTABLE_FLAG_PRESENT))
        return FALSE;

    if (((uint32_t)kernel_pde->ptable[pde] & PTABLE_FLAG_LARGEPAGE))
        return TRUE;

    pte = PTABLE_PTE_NUM(vaddr);

    return (kernel_page_tables[pde].entry[pte] & PTABLE_FLAG_PRESENT) ? TRUE : FALSE;
}

/****************
 * ptable_is_set
 **************** 
 * 
 * Checks if a page table exist for the given address
 * 
 * Inputs:
 *      - vaddr: virtual address to check
 * 
 * Returns:
 *      - TRUE if the table exists, FALSE otherwise 
 ***************/

static inline int ptable_is_set(vir_bytes vaddr)
{
    vir_clicks pde = PTABLE_PDE_NUM(vaddr);

    if (((uint32_t)kernel_pde->ptable[pde] & PTABLE_FLAG_LARGEPAGE))
        return FALSE;

    if ((uint32_t)kernel_pde->ptable[pde] & PTABLE_FLAG_PRESENT)
        return TRUE;

    return FALSE;
}

/****************
 * pstack_push
 **************** 
 * 
 * Pushes a free frame on the page stack. If the stack overflows a page boundary
 * the given frame is mapped in to contain the stack itself.
 * 
 * Inputs:
 *      - frame: physical address of the frame to push
 * 
 * Returns:
 *      - 0 on success, or nonzero on failure
 ***************/

static int pstack_push(phys_bytes frame)
{
    int result = 0;
    //check for stackoverflow
    if (pstack_pointer == KERNEL_PSTACK_BOTTOM)
        return 1;

    //check if we need to map in another page
    if (!(pstack_pointer & 0xFFFU) && !page_is_mapped(pstack_pointer - 1))
    {
        result = set_page(pstack_pointer - 1, frame, PTABLE_FLAG_PRESENT | PTABLE_FLAG_WRITE);
    }
    else
    {
        pstack_pointer -= sizeof(phys_bytes);
        *((phys_bytes *)pstack_pointer) = frame;
    }

    return result;
}
/****************
 * pstack_pop
 **************** 
 * 
 * pops a free frame from the page stack. If the stack shrinks below a page boundary,
 * that page is unmapped and returned.
 * 
 * Output:
 *      - frame: physical address of the frame that was popped
 * 
 * Returns:
 *      - 0 on success, or nonzero on failure
 ***************/

static int pstack_pop(phys_bytes *frame)
{
    int result;
    uint32_t flags;

    //check if we need to unmap
    if (!(pstack_pointer & 0xFFFU) && page_is_mapped(pstack_pointer - 1))
    {
        result = get_page(pstack_pointer - 1, frame, &flags);
        result |= set_page(pstack_pointer - 1, 0x00000000, 0x000);
        return result;
    }

    if (pstack_pointer >= KERNEL_PSTACK_TOP)
        return 1;

    *frame = *((phys_bytes *)pstack_pointer);
    pstack_pointer += sizeof(phys_bytes);

    return 0;
}

/****************
 * memory_init
 **************** 
 * 
 * Initializes the memory manager with the mmap data
 * retrieved from the global kinfo structure.
 *
 * Returns:
 *      - 0 on success, or nonzero on failure
 ***************/

int memory_init(void)
{
    phys_bytes end;
    phys_bytes frame;
    int counter = 0;

    /* setup static variables */
    kernel_pde = (struct page_directory *)KERNEL_PDE_ADDR;
    kernel_page_tables = (struct page_table *)KERNEL_PTABLE_ADDR;
    pstack_page_table = (struct page_table *)KERNEL_PSTACK_PTE;
    pstack_pointer = KERNEL_PSTACK_TOP;

    for (int i = 0; i < kinfo.mmap_size; i++)
    {
        if (kinfo.mmap[i].type != MMAP_TYPE_FREE)
            continue;
        frame = PAGE_ALIGN(kinfo.mmap[i].address);
        end = PAGE_FLOOR(kinfo.mmap[i].address + kinfo.mmap[i].size);
        while (frame < end)
        {
            pstack_push(frame);
            frame += PAGE_SIZE;
            counter++;
        }
    }

    /* setup heap */
    kernel_heap_addr = kinfo.kernel_end_vir;
    kernel_heap_size = KERNEL_INITIAL_HEAP_SIZE;

    for (vir_bytes i = 0; i < kernel_heap_size; i += PAGE_SIZE)
    {
        if (memory_allocate_page(kernel_heap_addr + i, FALSE, TRUE))
        {
            kpanic("Unable to map kernel heap.");
        }
    }
    libk_set_callback_sbrk(heap_callback);
    libk_init_heap((void *) kernel_heap_addr, kernel_heap_size);

    //unmap initial identity mapping
    for (vir_clicks i = 0; i < PTABLE_PDE_NUM(kinfo.kernel_start_vir); i++)
        kernel_pde->ptable[i] = 0;

    __asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %%cr3"
                         :
                         :
                         : "%eax", "memory");
    return 0;
}

/****************
 * memory_allocate_page
 **************** 
 * 
 * Allocates a page to the given address.
 * If the given addres is already mapped, an error will be returned
 * 
 * Inputs:
 *      - vaddr: virtual address to map-in a page
 *      - usermode: when set, the page is accessible by usermode processes
 *      - writable: when set, the page will be writable   
 * 
 * Returns:
 *      - 0 on success, or nonzero on failure
 * 
 * TODO: error numbers
 ***************/
int memory_allocate_page(vir_bytes vaddr, int usermode, int writable)
{
    phys_bytes frame, ptable;
    uint32_t flags;

    if (page_is_mapped(vaddr))
        return 1;

    if (pstack_pop(&frame))
        return 1;

    flags = PTABLE_FLAG_PRESENT;
    if (usermode)
        flags |= PTABLE_FLAG_USERMODE;
    if (writable)
        flags |= PTABLE_FLAG_WRITE;

    //determine if we need to set-up a page table to map the page
    if (!ptable_is_set(vaddr))
    {
        if (pstack_pop(&ptable))
        {
            pstack_push(frame);
            return 1;
        }
        set_page_table(vaddr, ptable, PTABLE_FLAG_PRESENT | PTABLE_FLAG_WRITE, TRUE);
    }

    set_page(vaddr, frame, flags);

    return 0;
}

/****************
 * memory_free_page
 **************** 
 * 
 * Deallocates a page to the given address.
 * If the given addres is not mapped, an error is returned
 * 
 * Inputs:
 *      - vaddr: virtual address of the page to deallocate
 * 
 * Returns:
 *      - 0 on success, or nonzero on failure
 * 
 * TODO: error numbers
 * TODO: cleanum empty page tables?
 ***************/

int memory_free_page(vir_bytes vaddr)
{
    uint32_t flags;
    phys_bytes frame;

    if (!page_is_mapped(vaddr))
        return 1;

    if (get_page(vaddr, &frame, &flags))
        return 1;

    if (set_page(vaddr, 0, 0))
        return 1;

    return pstack_push(frame);
}

/****************
 * memory_map_addr
 **************** 
 * 
 * Maps a virtual address to a given physical frame.
 * A page table is created for the address if needed.
 * 
 * Inputs:
 *      - vaddr: virtual address to map-in a page
 *      - frame: address of the physical frame map
 *      - usermode: when set, the page is accessible by usermode processes
 *      - writable: when set, the page will be writable 
 * 
 * Returns:
 *      - 0 on success, or nonzero on failure
 * 
 ***************/

int memory_map_addr(vir_bytes vaddr, phys_bytes frame, int usermode, int writable)
{
    phys_bytes ptable;
    uint32_t flags;

    if (page_is_mapped(vaddr))
        return 1;

    flags = PTABLE_FLAG_PRESENT;
    if (usermode)
        flags |= PTABLE_FLAG_USERMODE;
    if (writable)
        flags |= PTABLE_FLAG_WRITE;

    //determine if we need to set-up a page table to map the page
    if (!ptable_is_set(vaddr))
    {
        if (pstack_pop(&ptable))
            return 1;

        set_page_table(vaddr, ptable, PTABLE_FLAG_PRESENT | PTABLE_FLAG_WRITE, TRUE);
    }

    set_page(vaddr, frame, flags);

    return 0;
}

/****************
 * memory_unmap_addr
 **************** 
 * 
 * Unmaps a virtual address without deallocating the page
 * 
 * Inputs:
 *      - vaddr: virtual address of the page to unmap
 * 
 * Returns:
 *      - 0 on success, or nonzero on failure
 * 
 * TODO: error numbers
 * TODO: cleanum empty page tables?
 ***************/
int memory_unmap_addr(vir_bytes vaddr)
{
    if (!page_is_mapped(vaddr))
        return 1;

    return set_page(vaddr, 0, 0);
}

/****************
 * heap_callback
 **************** 
 * 
 * This function is called when the kernel heap needs to grow or schrink
 * It will map or unmap extra pages if possible
 * 
 * Inputs:
 *      - difference: Bytes to add to the heap. When negative, bytes are removed.
 * 
 * Returns:
 *      - 0 on success, or -1 on failure
 **************/
static void *heap_callback(intptr_t difference)
{
    size_t amount;
    vir_bytes new_heap_end;
    vir_bytes current_heap_end = kernel_heap_addr + kernel_heap_size;
    vir_bytes new_end_page;
    vir_bytes cur_end_page = PAGE_FLOOR(current_heap_end - 1);
    vir_bytes page;

    if (difference == 0)
        return (void *)0;

    if (difference < 0)  {
        amount = (size_t) (-difference);
        //check for underflow
        if(amount  > current_heap_end)
            return (void *)-1;

        //check to maintain at least minimal heap size
        if (kernel_heap_size - amount < KERNEL_INITIAL_HEAP_SIZE)
            return (void *)-1;

        new_heap_end = current_heap_end - amount;
        new_end_page = PAGE_FLOOR(new_heap_end - 1);
        
        for(page = cur_end_page; page > new_end_page; page -= PAGE_SIZE){
            memory_free_page(page);
        }
        kernel_heap_size -= amount;

    } else {
        amount = (size_t) difference;
        //check for maximum heap size and for overflow
        if (current_heap_end + amount >= KERNEL_HEAP_LIMIT)
            return (void *)-1;

        if(current_heap_end + amount < current_heap_end)
            return (void *)-1;

        new_heap_end = current_heap_end + amount;
        new_end_page = PAGE_FLOOR(new_heap_end - 1);

        int alloc_error = 0;
        for(page = cur_end_page + PAGE_SIZE; page <= new_end_page; page += PAGE_SIZE){
            alloc_error = memory_allocate_page(page, FALSE, TRUE);
            if(alloc_error)
                break;
        }

        //if there was an allocation error, rollback changes and return error
        if(alloc_error)
        {
            for(page = page - PAGE_SIZE; page > cur_end_page; page -= PAGE_SIZE){
                memory_free_page(page);
            }
            return (void *)-1;
        }

        kernel_heap_size += amount;
    }

  

    return (void *)0;
}
