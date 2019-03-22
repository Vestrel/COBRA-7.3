#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/syscall.h>
#include <lv2/io.h>
#include <lv2/synchronization.h>
#include <lv2/memory.h>

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
	
	//uint64_t flag2 = *(uint64_t*)((void*)tobj + 0x94);
	
	/*if(!(flag2 & ((1ul << 63) >> 25)))
		extend_kstack(0);
	else
	{
		//INFO("flag2= %lx vs %lx", flag2, newflag2);
	}*/

	sc_pool_elmnt_t *pe = NULL;

	// Simple filter for most spammy syscalls
	if (!( 
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
		num == SYS_LWMUTEX_LOCK ||
		num == 817 || // sys_fs_fcntl
		num == 801 || // sys_fs_open
		num == 802 // sys_fs_read
		))
	{
		// Suspend syscall
		suspend_intr();

		// Prepare a single memory representation of this structure, to avoid copying around data all the time
		if(num < MAX_NUM_OF_SYSTEM_CALLS) {
			syscall_info_t *info = get_syscall_info(num);
			
			if(info->trace) {
				pe = sc_pool_get_elmnt();
				if(pe != NULL) {
					pe->info = info;

					// We don't know the result yet
					pe->res     =    0;
					pe->has_res =    0;

					// But we know the parameters
					pe->args[0] =   r3;
					pe->args[1] =   r4;
					pe->args[2] =   r5;
					pe->args[3] =   r6;
					pe->args[4] =   r7;
					pe->args[5] =   r8;
					pe->args[6] =   r9;
					pe->args[7] =  r10;

					sc_send_pool_elmnt(pe);
				}
			}
		}
		else
		{
			ERROR("syscall_handler with num=%lx >= MAX_NUM_OF_SYSTEM_CALLS", num);
		}

		// End
		resume_intr();
	}

	uint64_t res = syscall(r3, r4, r5, r6, r7, r8, r9, r10);

	if(pe != NULL) {
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
