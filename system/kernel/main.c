#include "kernel.h"

#include <libk.h>
#include <kstring.h>
#include <kstdlib.h>
#include <cpuid.h>

#include <elco-os/multiboot.h>


static void get_mmap(multiboot_info_t *bootinfo);
void print_logo(void);


struct kinfo kinfo;

void kernel_early(uint32_t magic, multiboot_info_t *bootinfo)
{
	arch_init_display();
	libk_set_callback_putc(arch_display_putc);

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
	arch_init_tables();
	arch_init_interrupts();
	memory_init();
	multitasking_init();
	clock_setup();

}

task_t *tsk1;
task_t *tsk2;
task_t *tsk3;
task_t *tsk4;

/*
receive after send:
	timestep		task1	task2	task3	task4

	0				wait 1	wait 2	wait 3	wait	wait in order to get correct order
	1				1->3
	2						2->3
	3				wait2			rec All			should receive from 1, 1 unblocks
	4						wait2	rec ALL 		should receive from 2, 2 unblocks
	5				1->3
	6						2->3
	7						wait5	rec 2			should receive 2, 2 unblocks
	8				wait2			rec 1			should receive 1, 1 unblocks
send after receive
	9								rec All			blocks and waits
	10				1->3			rec 2			1 returns, 3 unblocks and wait for message from 2
	11				1->3							1 blocks, 3 stays blocked
	12						2->3					2 returns, 3 unblocks
	13								rec 1			3 returns, 1 unblocks			

*/



#define KC_SEND(pid, msg) (do_kernel_call(3, (uint32_t) (pid), (uint32_t) (msg)))
#define KC_REC(pid, msg) (do_kernel_call(4, (uint32_t) (pid), (uint32_t) (msg)))
#define KC_WAIT(ms) (do_kernel_call(0, (ms), 0))
#define KC_PREEMPT() (do_kernel_call(1, 0, 0))
#define STEP_MS 2000






void task1(void){
	int r;
	message_t mess;
    while(1){
	
		kprintf("t=0 t1 wait1\n");
		KC_WAIT(1 * STEP_MS);

		mess.contents[0] = 1;
		mess.contents[1] = 1;
		kprintf("t=1 t1->3\n");
		r = KC_SEND(3, &mess);
		if(r) kprintf("ERROR SENDING t1->3\n");
		
		kprintf("t=3 t1 wait2\n");
		KC_WAIT(2 * STEP_MS);

		mess.contents[0] = 1;
		mess.contents[1] = 2;
		kprintf("t=5 t1->3\n");
		r = KC_SEND(3, &mess);
		if(r) kprintf("ERROR SENDING t1->3\n");

		kprintf("t=8 t1 wait2\n");
		KC_WAIT(2 * STEP_MS);


		mess.contents[0] = 1;
		mess.contents[1] = 3;
		kprintf("t=10 t1->3\n");
		r = KC_SEND(3, &mess);
		if(r) kprintf("ERROR SENDING t1->3\n");

		kprintf("t=10 t1 wait1\n");
		KC_WAIT(1 * STEP_MS);

		mess.contents[0] = 1;
		mess.contents[1] = 4;
		kprintf("t=11 t1->3\n");
		r = KC_SEND(3, &mess);
		if(r) kprintf("ERROR SENDING t1->3\n");

		kprintf("t=13 t1 done\n");

		while(1){
			KC_PREEMPT();
		};
		
    }
}
void task2(void){
	int r;
	message_t mess;
    while(1){
	
		kprintf("t=0 t2 wait2\n");
		KC_WAIT(2 * STEP_MS);

		mess.contents[0] = 2;
		mess.contents[1] = 1;
		kprintf("t=2 t2->3\n");
		r = KC_SEND(3, &mess);
		if(r) kprintf("ERROR SENDING t2->3\n");
		
		kprintf("t=4 t2 wait2\n");
		KC_WAIT(2 * STEP_MS);

		mess.contents[0] = 2;
		mess.contents[1] = 2;
		kprintf("t=6 t2->3\n");
		r = KC_SEND(3, &mess);
		if(r) kprintf("ERROR SENDING t2->3\n");

		kprintf("t=7 t2 wait5\n");
		KC_WAIT(5 * STEP_MS);

		mess.contents[0] = 2;
		mess.contents[1] = 3;
		kprintf("t=12 t2->3\n");
		r = KC_SEND(3, &mess);
		if(r) kprintf("ERROR SENDING t2->3\n");

		kprintf("t=12 t2 done\n");
		while(1){KC_PREEMPT();}
		
    }
}

void task3(void){
	int r;
	message_t mess;
	kmemset(&mess, 0, sizeof(message_t));
    while(1){
	
		kprintf("t=0 t3 wait3\n");
		KC_WAIT(3 * STEP_MS);

		kprintf("t=3 t3 REC *\n");
		r = KC_REC(PID_ALL_TASKS, &mess);
		if(r) kprintf("ERROR REC t3\n");

		if(mess.contents[0] != 1) kprintf("t3 Rec error pid: %d\n", mess.contents[0]);
		if(mess.contents[1] != 1) kprintf("t3 Rec error num: %d\n", mess.contents[1]);
		kmemset(&mess, 0, sizeof(message_t));
		kprintf("t=3 t3 REC done\n");

		KC_WAIT(1 * STEP_MS);
		kprintf("t=4 t3 REC *\n");
		r = KC_REC(PID_ALL_TASKS, &mess);
		if(r) kprintf("ERROR REC t3\n");

		if(mess.contents[0] != 2) kprintf("t3 Rec error pid: %d\n", mess.contents[0]);
		if(mess.contents[1] != 1) kprintf("t3 Rec error num: %d\n", mess.contents[1]);
		kmemset(&mess, 0, sizeof(message_t));
		kprintf("t=4 t3 REC done\n");


		kprintf("t=4 t3 wait3\n");
		KC_WAIT(3 * STEP_MS);

		kprintf("t=7 t3 REC 2\n");
		r = KC_REC(2, &mess);
		if(r) kprintf("ERROR REC t3\n");

		if(mess.contents[0] != 2) kprintf("t3 Rec error pid: %d\n", mess.contents[0]);
		if(mess.contents[1] != 2) kprintf("t3 Rec error num: %d\n", mess.contents[1]);
		kmemset(&mess, 0, sizeof(message_t));
		kprintf("t=7 t3 REC done\n");

		KC_WAIT(1 * STEP_MS);

		kprintf("t=8 t3 REC 1\n");
		r = KC_REC(1, &mess);
		if(r) kprintf("ERROR REC t3\n");

		if(mess.contents[0] != 1) kprintf("t3 Rec error pid: %d\n", mess.contents[0]);
		if(mess.contents[1] != 2) kprintf("t3 Rec error num: %d\n", mess.contents[1]);
		kmemset(&mess, 0, sizeof(message_t));
		kprintf("t=8 t3 REC done\n");

		KC_WAIT(1 * STEP_MS);

		kprintf("t=9 t3 REC *\n");
		r = KC_REC(PID_ALL_TASKS, &mess);
		if(r) kprintf("ERROR REC t3\n");

		if(mess.contents[0] != 1) kprintf("t3 Rec error pid: %d\n", mess.contents[0]);
		if(mess.contents[1] != 3) kprintf("t3 Rec error num: %d\n", mess.contents[1]);
		kmemset(&mess, 0, sizeof(message_t));
		kprintf("t=10 t3 REC done\n");
		
		kprintf("t=10 t3 REC 2\n");
		r = KC_REC(2, &mess);
		if(r) kprintf("ERROR REC t3\n");

		if(mess.contents[0] != 2) kprintf("t3 Rec error pid: %d\n", mess.contents[0]);
		if(mess.contents[1] != 3) kprintf("t3 Rec error num: %d\n", mess.contents[1]);
		kmemset(&mess, 0, sizeof(message_t));
		kprintf("t=12 t3 REC done\n");
		KC_WAIT(1 * STEP_MS);

		kprintf("t=13 t3 REC 1\n");
		r = KC_REC(1, &mess);
		if(r) kprintf("ERROR REC t3\n");

		if(mess.contents[0] != 1) kprintf("t3 Rec error pid: %d\n", mess.contents[0]);
		if(mess.contents[1] != 4) kprintf("t3 Rec error num: %d\n", mess.contents[1]);
		kmemset(&mess, 0, sizeof(message_t));
		kprintf("t=13 t3 REC done\n");

		while(1) KC_PREEMPT();
		
    }
}

void task4(void){
    int a = 0;
    while(1){
        arch_display_number(a++, 50, 15);
		KC_WAIT(STEP_MS);
    }
}


static int get_model(void)
{
    int ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);
    return ebx;
}


void kernel_main(void)
{
 	print_logo();
	kdebug("started\n");
	kdebug("CPU Model: %x\n", get_model());

	int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);

	/*kdebug("CPU version eax: %x\n", eax);
	kdebug("CPU features ebx: %x\n", ebx);
	kdebug("CPU features ecx: %x\n", ecx);
	kdebug("CPU features edx: %x\n", edx);
	kdebug("CPU Model: %x\n", get_model());
	*/


	tsk1 = create_kernel_task(task1, 1024, 1);
	setPreemptible(tsk1, false);
	tsk2 = create_kernel_task(task2, 1024, 1);
	setPreemptible(tsk2, false);
	tsk3 = create_kernel_task(task3, 1024, 1);
	setPreemptible(tsk3, false);
	tsk4 = create_kernel_task(task4, 1024, 1);

	enqueue_task(tsk1);
	enqueue_task(tsk2);
	enqueue_task(tsk3);
	enqueue_task(tsk4);

	resume();



/* TODO:
	make simple message passing system via registers.
	message is saved in proctable of sending process
	sending process is blocked until delivered
	

*/





	//resume() never returns

	while(1);

}

void handle_kernel_call(void){
	int callnr = (int) current_task->context->eax;
	uint32_t arg1 = (uint32_t) current_task->context->ecx;
	uint32_t arg2 = (uint32_t) current_task->context->edx;

	int result = 0;


	switch(callnr){
		case 0:	//sleep
			sleep_ms((int) arg1);
			break;

		case 1:
			preempt();
			break;
		case 2:	//getpid
			result = current_task->pid;
			break;
		case 3:	//send
			result = message_send(current_task, (int32_t) arg1, (message_t *) arg2);
			break;
		case 4:	//receive
			result = message_receive(current_task, (int32_t) arg1, (message_t *) arg2);
			break;		
		default:
			break;
	}

	current_task->context->eax = (uint32_t) result;
}

void after_context_switch(void){
	if(current_task->flags & TASK_FLAGS_AFTER_SWITCH){
		current_task->flags &= (task_flags_t) ~TASK_FLAGS_AFTER_SWITCH;

		if(message_deliver(current_task)){
			current_task->context->eax = 0;
		}
	}
}




void kerror(const char* error) {
	kprintf("Kerror: %s\n", error);
}

void kpanic(const char* message) {
	kprintf("Kernel panic! - %s\n", message);
	//kabort();
	while(1);
}

void print_logo(void)
{
	char logo[800] = 
		"                                                                                "
		"             #### ##      ####    ####              ####     #####              "
		"             ##   ##    ##      ##    ##          ##    ##  ##                  "
		"             ###  ##    ##      ##    ##  ######  ##    ##   #####              "
		"             ##   ##    ##      ##    ##          ##    ##       ##             "
		"             #### ####    ####    ####              ####     #####              "
		"                                                                                ";
	
	arch_display_setcolor(COLOR_LIGHT_BROWN, COLOR_BLUE);
	kprintf(logo);
	arch_display_setcolor(COLOR_LIGHT_GREY, COLOR_BLACK);
	kprintf("\n");

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