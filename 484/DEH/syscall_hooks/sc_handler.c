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
static void sc_add_process_and_thread_info(sc_pool_elmnt_t *pe, char **out_proc_nm, char **out_thrd_nm) {
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
			*out_proc_nm = proc_nm;
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
				*out_thrd_nm = "MAIN";
				sc_pe_add_str(pe, "MAIN");
			}
			else {
				// The thread name is usually e.g. <PID>_main_<THRD_NAME> 
				char *thrd_nm = tobj->name;

				// Remove the ID if it matches the process name
				if(orig_proc_nm != NULL && strncmp(thrd_nm, orig_proc_nm, 8) == 0)
					thrd_nm = thrd_nm + 8;
				
				// Remove the "_" (if present)
				if(thrd_nm[0] == '_')
					thrd_nm += 1;

				// Remove the "_main_" (if present)
				if(strncmp(thrd_nm, "main_", 5) == 0)
					thrd_nm += 5;

				// Submit
				*out_thrd_nm = thrd_nm;
				sc_pe_add_str(pe, thrd_nm);
			}
		#endif
	}
}

static sc_pool_elmnt_t* __attribute__((noinline)) handle_precall(sc_pool_elmnt_t *pe, char **_proc_nm, char **_thrd_nm, syscall_trace_e should_trace) {
	suspend_intr();

	syscall_info_t *info = pe->info;

	// We don't know the result yet
	pe->res     =    0;
	pe->has_res =    0;

	// Process/Thread information
	char *proc_nm = NULL;
	char *thrd_nm = NULL;
	sc_add_process_and_thread_info(pe, &proc_nm, &thrd_nm);

	// Callbacks / Filtering
	int skip = 0;

	if(proc_nm != NULL && thrd_nm != NULL) {
		if(strcmp(proc_nm, "vsh.self") == 0) {
			skip |= strcmp(thrd_nm, "aud_SERIAL" ) == 0 ||
					strcmp(thrd_nm, "aud_BS"     ) == 0 ||
					strcmp(thrd_nm, "SceVshPower") == 0;
		}
	}

	#ifndef SC_HANDLER_NO_CALLBACKS
		if(!skip) {
			sc_handler_callback *cb = info->precall_prepare_cb;
			if(cb && cb(pe, proc_nm, thrd_nm) != 0)
				skip = 1;
		}
	#endif

	// Submit parameters
	if(!skip) {
		*_proc_nm = proc_nm;
		*_thrd_nm = thrd_nm;

		if(should_trace != SCT_TRACE_POST)
			sc_send_pool_elmnt(pe);
	}
	else {
		sc_pe_unlock(pe);
		pe = NULL;
	}

	resume_intr();

	return pe;
}

static void __attribute__((noinline)) handle_postcall(sc_pool_elmnt_t *pe, uint64_t r4, uint64_t r5, uint64_t r6, uint64_t r7, char *proc_nm, char *thrd_nm) {
	suspend_intr();

	syscall_info_t *info = pe->info;

	pe->res_regs[0] = r4;
	pe->res_regs[1] = r5;
	pe->res_regs[2] = r6;
	pe->res_regs[3] = r7;

	int skip = 0;

	for(uint16_t i = 0; i < 8; i++) {
		switch(info->arg_ptr[i]) {
			case 1: pe->args_ptd[i] = (uint64_t)(*(uint8_t*)(pe->args[i])); break;
			case 2: pe->args_ptd[i] = (uint64_t)(*(uint16_t*)(pe->args[i])); break;
			case 4: pe->args_ptd[i] = (uint64_t)(*(uint32_t*)(pe->args[i])); break;
			case 8: pe->args_ptd[i] = (uint64_t)(*(uint64_t*)(pe->args[i])); break;
		}
	}

	#ifndef SC_HANDLER_NO_CALLBACKS
		sc_handler_callback *cb = info->postcall_prepare_cb;
		if(cb && cb(pe, proc_nm, thrd_nm) != 0)
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
	resume_intr();
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

	// Trace the syscall
	sc_pool_elmnt_t *pe = NULL;
	uint32_t pe_uid = 0;
	char *proc_nm = NULL;
	char *thrd_nm = NULL;
	syscall_trace_e should_trace = SCT_DEFAULT;

	if(num < MAX_NUM_OF_SYSTEM_CALLS) {
		syscall_info_t *info = sci_get_sc(num);
		should_trace = sci_get_trace(info);
		
		if(should_trace != SCT_DONT_TRACE) {
			pe = sc_pool_get_elmnt();
			if(pe != NULL) {
				pe_uid = pe->uid;
				pe->info = info;

				// But we know the parameters
				pe->args[0] =  r3;
				pe->args[1] =  r4;
				pe->args[2] =  r5;
				pe->args[3] =  r6;
				pe->args[4] =  r7;
				pe->args[5] =  r8;
				pe->args[6] =  r9;
				pe->args[7] = r10;

				pe = handle_precall(pe, &thrd_nm, &proc_nm, should_trace);
			}
		}
	}
	else
	{
		ERROR("syscall_handler with num=%hx >= MAX_NUM_OF_SYSTEM_CALLS", num);
	}

	// Do actual syscall
	uint64_t res = syscall(r3, r4, r5, r6, r7, r8, r9, r10);

	#ifndef SC_AVOID_LARGE_STACK
		volatile register uint64_t _ret_r4 asm("r4");
		volatile register uint64_t _ret_r5 asm("r5");
		volatile register uint64_t _ret_r6 asm("r6");
		volatile register uint64_t _ret_r7 asm("r7");

		uint64_t ret_r4 = _ret_r4;
		uint64_t ret_r5 = _ret_r5;
		uint64_t ret_r6 = _ret_r6;
		uint64_t ret_r7 = _ret_r7;
	#endif

	// Store result (if we are still alive here, not all syscalls return)
	if(pe != NULL && pe->uid == pe_uid) {
		pe->res = res;
		pe->has_res = 1;

		if(should_trace == SCT_TRACE_POST) {
			#ifndef SC_AVOID_LARGE_STACK
				handle_postcall(pe, ret_r4, ret_r5, ret_r6, ret_r7, proc_nm, thrd_nm);
			#else
				handle_postcall(pe, 0, 0, 0, 0, proc_nm, thrd_nm);
			#endif
		}
	}

	#ifndef SC_AVOID_LARGE_STACK
		__asm__ __volatile__ (
			"mr %[ret4],%[in4] \n\t"
			"mr %[ret5],%[in5] \n\t"
			"mr %[ret6],%[in6] \n\t"
			"mr %[ret7],%[in7] \n\t"
			: [ret4] "=&r" (_ret_r4), [ret5] "=&r" (_ret_r5), [ret6] "=&r" (_ret_r6), [ret7] "=&r" (_ret_r7)
			: [in4] "r" (ret_r4), [in5] "r" (ret_r5), [in6] "r" (ret_r6), [in7] "r" (ret_r7)
		);
	#endif

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
