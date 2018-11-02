#include "kernel.h"
#include <kstring.h>

#include "multiboot.h"
#include "i386/i386.h"


static void get_mmap(multiboot_info_t *bootinfo);

struct kinfo kinfo;

void kernel_early(uint32_t magic, multiboot_info_t *bootinfo)
{
	i386_init();

	if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
		kpanic("Elco-OS should be loaded with a valid multiboot bootloader!\n");
	}

	/* setup kinfo */

	kinfo.kernel_start_vir = (vir_bytes) _start_of_kernel;
	kinfo.kernel_start_phys = (phys_bytes) _start_of_kernel_phys;
	kinfo.kernel_end_vir = (vir_bytes) _end_of_kernel;
	kinfo.kernel_end_phys = (phys_bytes) _end_of_kernel_phys;


	if(bootinfo->flags & MULTIBOOT_INFO_CMDLINE) {
		kstrncpy(kinfo.commandline, (char *) bootinfo->cmdline, 127);
		kinfo.commandline[127] = '\0';
	} else {
		kstrcpy(kinfo.commandline, "");
	}

	get_mmap(bootinfo);
	
	kdebug("Memmap:\n");

	phys_bytes free = 0;
	for(int i = 0; i < kinfo.mmap_size; i++){
		kdebug ("\tStart: %x", kinfo.mmap[i].address);
		kdebug ("\tlen: %x", kinfo.mmap[i].size);
		kdebug ("\ttype: %u\n", kinfo.mmap[i].type);
		if(kinfo.mmap[i].type == 1)
			free += kinfo.mmap[i].size;
	}

	kdebug("%u entries, %u MB available\n", kinfo.mmap_size, free >> 20);
	memory_init();

}

void kernel_main(void)
{
	int t1 = memory_allocate_page(0x1000,0,0);
	int t2 = memory_allocate_page(0x2000,1,0);
	int t3 = memory_allocate_page(0x3000,0,1);
	int t4 = memory_allocate_page(0x1000,1,1);

	kdebug("t1 %d t2 %d t3 %d t4 %d\n", t1, t2, t3, t4);

	t1 = memory_free_page(0x1000);
	t2 = memory_unmap_addr(0x2000);
	t3 = memory_map_addr(0xB0000000, 0x1000,0,0);
	kdebug("%d %d %d\n", t1, t2, t3);

 	terminal_print_logo();
	kdebug("started\n");

	while(1);

}

void kerror(const char* error) {
	terminal_writestring("Kerror: ");
	terminal_writestring(error);
}

void kpanic(const char* message) {
	terminal_writestring("Kernel panic! - ");
	terminal_writestring(message);
	//kabort();
	while(1);
}



static void get_mmap(multiboot_info_t *bootinfo) {

	struct change_point {
		phys_bytes addr;
		struct multiboot_mmap_entry *entry;
	};
	struct change_point change_point_list[2 * MAX_MMAP_SIZE];
	struct change_point *changes[2 * MAX_MMAP_SIZE];
	struct multiboot_mmap_entry *overlap[MAX_MMAP_SIZE];
	struct change_point *change_tmp;
	int is_changing, mmap_id, overlap_count;
	phys_bytes last_addr;
	uint32_t last_type, cur_type;
	int chxcount = 0;

	multiboot_memory_map_t *bootmap;
	multiboot_memory_map_t dummy;

	if(!(bootinfo->flags & MULTIBOOT_INFO_MEM_MAP))
		kpanic("No memory information provided by bootloader!\n");

	//loop through memmap and save changes in list
	bootmap = (multiboot_memory_map_t *) bootinfo->mmap_addr;

	while((phys_bytes) bootmap < (bootinfo->mmap_addr + bootinfo->mmap_length))
    {
		//we use only 32bit physical space, so ignore anything above that
		if(bootmap->addr_high == 0) {
			change_point_list[chxcount].addr = bootmap->addr_low;
			change_point_list[chxcount++].entry = bootmap;

			if(bootmap->len_high > 0 || (bootmap->addr_low + bootmap->len_low < bootmap->addr_low))
				change_point_list[chxcount].addr = PHYS_BYTES_MAX;
			else
				change_point_list[chxcount].addr = bootmap->addr_low + bootmap->len_low;
			change_point_list[chxcount++].entry = bootmap;
		}
        bootmap = (multiboot_memory_map_t *) ( (phys_bytes) bootmap + bootmap->size + sizeof(bootmap->size) );
		if(chxcount > MAX_BOOT_MMAP_SIZE)
			kpanic("Memory map overflow! Too many entries!\n");
    }

	//add changes for the kernel area itself, mark it as higest possible type
	dummy.addr_high = 0;
	dummy.addr_low = kinfo.kernel_start_phys;
	dummy.len_high = 0;
	dummy.len_low = kinfo.kernel_end_phys - kinfo.kernel_start_phys;
	dummy.type = MMAP_TYPE_KERNEL;
	change_point_list[chxcount].addr = kinfo.kernel_start_phys;
	change_point_list[chxcount++].entry = &dummy;
	change_point_list[chxcount].addr = kinfo.kernel_end_phys;
	change_point_list[chxcount++].entry = &dummy;	
	
	//initialize changes to point to the list
	for(int i = 0; i < chxcount; i++)
		changes[i] = &change_point_list[i];

	//sort changes
	is_changing = 1;
	while (is_changing)	{
		is_changing = 0;
		for (int i=1; i < chxcount; i++)  {
			//swap if address is smaller than previous,
			//if both changes are form the same address, place end change after start change
			if ((changes[i]->addr < changes[i-1]->addr) ||
				((changes[i]->addr == changes[i-1]->addr) &&
				 (changes[i]->addr == changes[i]->entry->addr_low) &&
				 (changes[i-1]->addr != changes[i-1]->entry->addr_low))
			   )
			{
				change_tmp = changes[i];
				changes[i] = changes[i-1];
				changes[i-1] = change_tmp;
				is_changing =1;
			}
		}
	}

	//detect overlaps and create sorted and sanitized map
	overlap_count = 0;
	mmap_id = 0;
	last_addr = 0;
	last_type = cur_type = 0;

	for(int i = 0; i < chxcount; i++){

		if(changes[i]->addr == changes[i]->entry->addr_low){	//if this is a start of a entry
			overlap[overlap_count] = changes[i]->entry;
			overlap_count++;
		}else{		//this is an end of an entry
			for(int j = 0; j < overlap_count; j++){
				if(overlap[j] == changes[i]->entry){
					overlap[j] = overlap[overlap_count - 1];
					break;
				}
			}
			overlap_count--;
		}

		//find 'overruling' type of current regio
		cur_type = 0;
		for(int j = 0; j < overlap_count; j++)
			if(overlap[j]->type > cur_type)
				cur_type = overlap[j]->type;
		
		//if type has changed, update map
		if(cur_type != last_type){
			if(last_type != 0){	
				kinfo.mmap[mmap_id].size = changes[i]->addr - last_addr;
				if(kinfo.mmap[mmap_id].size != 0)
					if(++mmap_id >= MAX_MMAP_SIZE){
						kerror("Too many entries for kinfo::mmap, truncating...");
						break;
					}
			}

			if(cur_type != 0){
				kinfo.mmap[mmap_id].address = changes[i]->addr;
				kinfo.mmap[mmap_id].type = cur_type;
				last_addr = changes[i]->addr;
			}
			last_type = cur_type;
		}
	}

	kinfo.mmap_size = mmap_id;

}