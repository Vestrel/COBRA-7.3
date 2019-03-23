#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/syscall.h>
#include <lv2/io.h>
#include <lv2/synchronization.h>
#include <lv2/memory.h>
#include <lv2/thread.h>
#include <lv2/hw_cur.h>

#include "sc_handler.h"
#include "sc_pool.h"
#include "sc_writer.h"
#include "sc_info.h"
#include "utilities.h"


/*
 * Helper methods
 */
static void sc_add_process_and_thread_info(sc_pool_elmnt_t *pe) {
	// HWT
	pe->hwt = get_HW_cur()->id;

	// Process
	process_t proc = get_current_process();
	#ifndef SC_LOG_MINIMUM
		char *orig_proc_nm = NULL;
	#endif

	// Fast-path for NULL case
	if(proc == NULL) {
		pe->pid = 0;
		#ifndef SC_LOG_MINIMUM
			sc_pe_add_str(pe, NULL);
		#endif
	}
	else {
		// PID
		pe->pid = proc->pid;

		#ifndef SC_LOG_MINIMUM
			// get_process_name() returns e.g. <PID>_main_vsh.self 
			orig_proc_nm = get_process_name(proc);

			// Remove the ID
			char *proc_nm = orig_proc_nm + 8;

			// Remove the _main_
			if(strncmp(proc_nm, "_main_", 6) == 0)
				proc_nm += 6;

			// Remove the '.self'
			char *dot = strrchr(proc_nm, '.');
			if(dot != NULL && strncmp(dot, ".self", 5) == 0)
				*dot = '\0';
			else
				dot = NULL;

			// Submit
			sc_pe_add_str(pe, proc_nm);

			// Add the '.' back
			if(dot != NULL)
				*dot = '.';
		#endif
	}
	
	// Thread
	thread_obj_t *tobj = get_thread_obj();

	if(!tobj) {
		pe->tid = 0;
		#ifndef SC_LOG_MINIMUM
			sc_pe_add_str(pe, NULL);
		#endif
		return;
	}
	else {
		pe->tid = tobj->id;

		#ifndef SC_LOG_MINIMUM
			if(orig_proc_nm != NULL && strcmp(tobj->name, orig_proc_nm) == 0) {
				sc_pe_add_str(pe, "MAIN");
			}
			else {
				// The thread name is usually e.g. <PID>_main_<THRD_NAME> 
				char *thrd_nm = tobj->name;

				// Remove the ID if it matches the process name
				if(orig_proc_nm != NULL && strncmp(thrd_nm, orig_proc_nm, 8) == 0)
					thrd_nm = thrd_nm + 8;

				// Remove the "_main_"
				if(strncmp(thrd_nm, "_main_", 6) == 0)
					thrd_nm += 6;

				sc_pe_add_str(pe, tobj->name);
			}
		#endif
	}
}


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

	// Prepare a single memory representation of this structure, to avoid copying around data all the time
	if(num < MAX_NUM_OF_SYSTEM_CALLS) {
		syscall_info_t *info = get_syscall_info(num);
		
		if(info->trace) {
			pe = sc_pool_get_elmnt();
			suspend_intr();
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

				// TODO: HW Thread number

				// Process/Thread information
				sc_add_process_and_thread_info(pe);

				//Callback
				int skip = 0;
				#ifndef SC_HANDLER_NO_CALLBACKS
					sc_callback *cb = info->prepare_cb;
					if(cb && cb(pe) != 0)
						skip = 1;
				#endif

				// Submit parameters
				if(!skip) {
					sc_send_pool_elmnt(pe);
				}
				else {
					sc_pe_unlock(pe);
					pe = NULL;
				}
			}
			resume_intr();
		}
	}
	else
	{
		ERROR("syscall_handler with num=%hx >= MAX_NUM_OF_SYSTEM_CALLS", num);
	}

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
