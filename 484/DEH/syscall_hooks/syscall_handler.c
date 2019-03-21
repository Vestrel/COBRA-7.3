#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/io.h>
#include <lv2/synchronization.h>
#include <lv2/memory.h>

#include "syscall_handler.h"
#include "syscall_writer.h"
#include "utilities.h"


/*
 * Default printer
 */
void default_syscall_printer(syscall_t *sc)
{
	syscall_info_t *info = sc->info;

	if(!info->trace)
		return;

	// Prepare variables
	char *nm     = info->name;
	char *proc   = sc->proc;
	//uint64_t thr = 0;//(uint64_t)sc->thr;
	uint64_t res = sc->res;

	// Grab buffer
	char *scb = (char*)malloc(SC_POOL_BUF_LEN);
	if(scb == NULL) {
		ERROR("Failed to allocate char*scb");
		return;
	}
	int len = 0;

	#define WRITE(fmt, ...) { \
		int remaining = SC_POOL_BUF_LEN - len - 1; \
		if(remaining > 0) { \
			char *cur = scb + len; \
			len += snprintf(cur, remaining+1, fmt, ## __VA_ARGS__ ); \
		} \
	} while(0)

	// Print
	WRITE("%s %s(", proc, nm);
	
	free(scb);

	/*uint16_t nargs = info->nargs;
	for(uint16_t i = 0; i < nargs; i++) {
		if(i > 0)
			WRITE(", ");

		WRITE(info->arg_fmt[i], sc->args[i]);
	}

	WRITE(") -> %lx\n", res);*/

	// send
}


/*
 * Syscall handler
 */
static void on_syscall(syscall_t *sc)
{
	// Prepare missing information
	char *proc = get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL";
	if(strncmp(proc, "_main_", 6) == 0)
		proc += 6;
	sc->proc = proc;

	//sc->thr  = get_current_thread();

	sc->info = get_syscall_info(sc->num);

	SC_PRINTF(64, "sc %lx\n", sc->num);

	// Trigger callback (if exists)
	//sci_callback *cb = sc->info->cb;
	//if(cb && cb(sc))
	//	return;

	// Default printer
	//default_syscall_printer(sc);
}


/*
 * Main syscall handler
 */
LV2_PATCHED_FUNCTION(uint64_t, syscall_handler, (uint64_t r3, uint64_t r4, uint64_t r5, uint64_t r6, uint64_t r7, uint64_t r8, uint64_t r9, uint64_t r10))
{
	// Figure out arguments, original syscall location, etc
	register uint64_t r11 asm("r11");
	register uint64_t r13 asm("r13");
	uint64_t num, p;
	f_desc_t func;

	num = r11/8;
	p = r13;

	func.addr = (void *)p;
	func.toc = (void *)MKA(TOC);

	uint64_t (* syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = (void *)&func;
	
	//thread_obj_t *tobj = get_thread_obj();
	//uint64_t flag2 = *(uint64_t*)((void*)tobj + 0x94);

	// Call original syscall and store result
	uint64_t res = syscall(r3, r4, r5, r6, r7, r8, r9, r10);
	
	//uint64_t flag2 = *(uint64_t*)((void*)tobj + 0x94);
	
	/*if(!(flag2 & ((1ul << 63) >> 25)))
		extend_kstack(0);
	else
	{
		//INFO("flag2= %lx vs %lx", flag2, newflag2);
	}*/

	// Simple filter for most spammy syscalls
	if ( 
		num == 0x363 || // 867 sys_ss_appliance_info_manager (breaks things if we don't return immediately)
		num == SYS_SM_GET_EXT_EVENT2 ||
		num == SYS_SPU_THREAD_GROUP_START ||
		num == SYS_SPU_THREAD_GROUP_JOIN ||
		num == SYS_EVENT_QUEUE_RECEIVE ||
		num == SYS_EVENT_PORT_SEND ||
		num == SYS_MUTEX_UNLOCK ||
		num == SYS_MUTEX_LOCK ||
		num == 579 || //sys_bluetooth_aud_serial_unk1
		num == SYS_RWLOCK_RLOCK ||
		num == SYS_RWLOCK_WLOCK ||
		num == SYS_RWLOCK_RUNLOCK ||
		num == SYS_LWCOND_QUEUE_WAIT ||
		num == SYS_TIMER_USLEEP ||
		num == SYS_RWLOCK_WLOCK ||
		num == SYS_RWLOCK_WUNLOCK ||
		//num == SYS_HID_MANAGER_CHECK_FOCUS ||
		num == SYS_NET_RECVFROM ||
		num == SYS_NET_SELECT ||
		num == SYS_LWMUTEX_LOCK
		)
	{
		return res;
	}

	// Suspend syscall
	suspend_intr();
	//uint64_t irq = spin_lock_irqsave();

	// Prepare a single memory representation of this structure, to avoid copying around data all the time
	if(num < MAX_NUM_OF_SYSTEM_CALLS) {
		syscall_t sc;

		sc.num = num;
		sc.res = res;
		sc.args[0] =  r3;
		sc.args[1] =  r4;
		sc.args[2] =  r5;
		sc.args[3] =  r6;
		sc.args[4] =  r7;
		sc.args[5] =  r8;
		sc.args[6] =  r9;
		sc.args[7] = r10;

		// Dump syscall
		on_syscall(&sc);
		
		res = sc.res;
	}
	else
	{
		ERROR("syscall_handler with num=%lx >= MAX_NUM_OF_SYSTEM_CALLS", num);
	}

	// End
	//spin_unlock_irqrestore(irq);
	resume_intr();

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
