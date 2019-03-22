#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/syscall.h>
#include <lv2/io.h>
#include <lv2/synchronization.h>
#include <lv2/memory.h>
#include <lv2/thread.h>

#include "sc_handler.h"
#include "sc_pool.h"
#include "sc_writer.h"
#include "sc_info.h"
#include "utilities.h"


/*
 * Main syscall handler
 */
LV2_PATCHED_FUNCTION(uint64_t, syscall_handler, (uint64_t r3, uint64_t r4, uint64_t r5, uint64_t r6, uint64_t r7, uint64_t r8, uint64_t r9, uint64_t r10))
{
	// Figure out arguments, original syscall location, etc
	register uint64_t r11 asm("r11");
	register uint64_t r13 asm("r13");

	uint64_t p = r13;
	uint16_t num = (uint16_t)(r11/8);

	f_desc_t func;
	func.addr = (void *)p;
	func.toc = (void *)MKA(TOC);

	uint64_t (* syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = (void *)&func;
	
	//thread_obj_t *tobj = get_thread_obj();
	//uint64_t flag2 = *(uint64_t*)((void*)tobj + 0x94);

	// Call original syscall and store result
	
	//uint64_t flag2 = *(uint64_t*)((void*)tobj + 0x94);
	
	/*if(!(flag2 & ((1ul << 63) >> 25)))
		extend_kstack(0);
	else
	{
		//INFO("flag2= %lx vs %lx", flag2, newflag2);
	}*/

	sc_pool_elmnt_t *pe = NULL;
	uint32_t pe_uid = 0;

	// Suspend syscall
	suspend_intr();

	// Prepare a single memory representation of this structure, to avoid copying around data all the time
	if(num < MAX_NUM_OF_SYSTEM_CALLS) {
		syscall_info_t *info = get_syscall_info(num);
		
		if(info->trace) {
			pe = sc_pool_get_elmnt();
			if(pe != NULL) {
				pe->info = info;
				pe_uid = pe->uid;

				// We don't know the result yet
				pe->res     =    0;
				pe->has_res =    0;

				// But we know the parameters
				pe->args[0] =  r3;
				pe->args[1] =  r4;
				pe->args[2] =  r5;
				pe->args[3] =  r6;
				pe->args[4] =  r7;
				pe->args[5] =  r8;
				pe->args[6] =  r9;
				pe->args[7] = r10;
				
				// Processor name
				process_t proc = get_current_process();
				char *proc_nm;
				char *dot = NULL;
				if(proc) {
					 // get_process_name() returns e.g. 01000300_main_vsh.self 
					proc_nm = get_process_name(proc);

					// Remove the ID
					proc_nm += 8;

					// Remove the _main_
					if(strncmp(proc_nm, "_main_", 6) == 0)
						proc_nm += 6;

					// Remove the '.self'
					dot = strrchr(proc_nm, '.');
					if(dot != NULL && strncmp(dot, ".self", 5) == 0)
						*dot = '\0';
					else
						dot = NULL;
				}
				else
					proc_nm = "KERNEL";

				// TODO: Allow filtering by process

				sc_pe_add_str(pe, proc_nm);
				
				// TODO: Call pe->info->prepare_cb

				// Finish up, and send it to the writer thread!
				sc_send_pool_elmnt(pe);

				// Restore proc_nm dot
				if(dot != NULL)
					*dot = '.';
			}
		}
	}
	else
	{
		ERROR("syscall_handler with num=%hx >= MAX_NUM_OF_SYSTEM_CALLS", num);
	}

	// End
	resume_intr();

	// Do actual syscall
	uint64_t res = syscall(r3, r4, r5, r6, r7, r8, r9, r10);

	// Store result (if we are still alive here, not all syscalls return)
	if(pe != NULL && pe->uid == pe_uid) {
		pe->res = res;
		pe->has_res = 1;
	}

	return res;
}



/*
 * Initialization
 */
int init_syscall_handler(void)
{
	// Hook syscalls
	set_syscall_handler(syscall_handler);

	// Done
	return 0;
}
