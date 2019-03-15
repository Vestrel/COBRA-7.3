#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lv2/lv2.h>
#include <lv2/libc.h>
#include <lv2/memory.h>
#include <lv2/patch.h>
#include <lv2/syscall.h>
#include <lv2/usb.h>
#include <lv2/storage.h>
#include <lv2/thread.h>
#include <lv2/synchronization.h>
#include <lv2/modules.h>
#include <lv2/io.h>
#include <lv2/time.h>
#include <lv2/security.h>
#include <lv2/pad.h>
#include <lv2/error.h>
#include <lv2/symbols.h>
#include <lv2/thread.h>
#include <lv1/stor.h>
#include <lv1/patch.h>

#include "common.h"

/*
 * Syscall Printing Helpers
 */
#define STR(x) ((x) ? (char*)(x) : "NULL")

/*
 * Main syscall handler
 */
LV2_PATCHED_FUNCTION(uint64_t, syscall_handler, (uint64_t r3, uint64_t r4, uint64_t r5, uint64_t r6, uint64_t r7, uint64_t r8, uint64_t r9, uint64_t r10))
{
	register uint64_t r11 asm("r11");
	register uint64_t r13 asm("r13");
	uint64_t num, p;
	f_desc_t func;

	num = r11/8;
	p = r13;

	func.addr = (void *)p;
	func.toc = (void *)MKA(TOC);
	uint64_t (* syscall)() = (void *)&func;

	suspend_intr();

	if (1)
	{
		/*if (num == 378)
		{
			uint64_t *p1, *p2, *p3;

			p1 = (uint64_t *)r3;
			p2 = (uint64_t *)r4;
			p3 = (uint64_t *)r5;

			resume_intr();
			uint64_t ret = syscall(r3, r4, r5, r6);

			DPRINTF("ret=%lx r3 %016lx r4 %016lx r5 %016lx\n", ret, *p1, *p2, *p3);
			// r3 = 3 -> power off button pressed
			if (*p1 == 3)
			{
				// Change to reboot :)
				*p1 = 5; *p2 = 2;
			}
			return ret;
		}*/
		// Prepare your self for a big amount of data in your terminal!
		// And big ps3 slowdown
		// Uncomment the if to skip some too common syscalls and make things faster and smaller

		/*if (num != 378 && num != 173 && num != 178 && num != 130 && num != 138 && num != 104 && num != 102 && num != 579 && num != 122 && num != 124
			&& num != 113 && num != 141 && num != 125 && num != 127 && num != 141) */
		if (
			num != SYS_SM_GET_EXT_EVENT2
			&& num != SYS_SPU_THREAD_GROUP_START
			&& num != SYS_SPU_THREAD_GROUP_JOIN
			&& num != SYS_EVENT_QUEUE_RECEIVE
			&& num != SYS_EVENT_PORT_SEND
			&& num != SYS_MUTEX_UNLOCK
			&& num != SYS_MUTEX_LOCK
			&& num != 579 /*sys_bluetooth_aud_serial_unk1*/
			&& num != SYS_RWLOCK_RLOCK
			&& num != SYS_RWLOCK_RUNLOCK
			&& num != SYS_LWCOND_QUEUE_WAIT
			&& num != SYS_TIMER_USLEEP
			&& num != SYS_RWLOCK_WLOCK
			&& num != SYS_RWLOCK_WUNLOCK
			//&& num != SYS_HID_MANAGER_CHECK_FOCUS
			&& num != SYS_NET_RECVFROM
			&& num != SYS_NET_SELECT
			&& num != SYS_LWMUTEX_LOCK
			)
		{

			if(num == SYS_HID_MANAGER_CHECK_FOCUS) {
				lv2_printf("%s syscall sys_hid_manager_check_focus()\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL");
			}
			else if(num == SYS_COND_SIGNAL) {
				lv2_printf("%s syscall sys_cond_signal(cond_id=0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3);
			}
			else if(num == SYS_MEMORY_ALLOCATE) {
				lv2_printf("%s syscall sys_memory_allocate(size=0x%lx, flags=0x%lx, alloc_addr=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5);
			}
			else if(num == SYS_LWMUTEX_LOCK) {
				lv2_printf("%s syscall sys_lwmutex_lock(lwmutex=*0x%lx, timeout=0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4);
			}
			else if(num == SYS_LWMUTEX_UNLOCK) {
				lv2_printf("%s syscall sys_lwmutex_unlock(lwmutex=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3);
			}
			else if(num == SYS_MEMORY_GET_PAGE_ATTRIBUTE) {
				lv2_printf("%s syscall sys_memory_get_page_attribute(addr=0x%lx, attr=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4);
			}
			else if(num == SYS_NET_INFOCTL) {
				lv2_printf("%s syscall sys_net_infoctl(cmd=%ld, args=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4);
			}
			else if(num == SYS_NET_ABORT) {
				lv2_printf("%s syscall sys_net_abort(type=%ld, arg=0x%lx, flags=0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5);
			}
			else if(num == SYS_NET_IOCTL) {
				lv2_printf("%s syscall sys_net_bnet_ioctl(socket_id=%ld, command=0x%lx, args=0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5);
			}
			else if(num == 514) {
				lv2_printf("%s syscall sys_hid_manager_514(%lx %lx %lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5);
			}
			else if(num == _SYS_PRX_START_MODULE) {
				lv2_printf("%s syscall sys_prx_start_module(id=0x%lx, args=%lu, argp=*0x%lx, result=*0x%lx, flags=0x%lx, pOpt=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6, r7, r8);
			}
			else if(num == SYS_HID_MANAGER_READ) {
				lv2_printf("%s syscall sys_hid_manager_read(handle=0x%lx, pkg_id=0x%lx, buf=*0x%lx, buf_size=0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6);
			}
			else if(num == SYS_LWCOND_SIGNAL) {
				lv2_printf("%s syscall sys_lwcond_signal(lwcond=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3);
			}
			else if(num == SYS_EVENT_QUEUE_DRAIN) {
				lv2_printf("%s syscall sys_event_queue_drain(equeue_id=0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3);
			}
			else if(num == SYS_NET_SHUTDOWN) {
				lv2_printf("%s syscall sys_net_bnet_shutdown(s=%ld, how=%ld)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4);
			}
			else if(num == SYS_PPU_THREAD_CREATE) {
				lv2_printf("%s syscall _sys_ppu_thread_create(thread_id=*0x%lx, param=*0x%lx, arg=0x%lx, unk=0x%lx, prio=%ld, stacksize=0x%lx, flags=0x%lx, threadname=%s)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6, r7, r8, r9, STR(r10));
			}
			else if(num == SYS_PPU_THREAD_RENAME) {
				lv2_printf("%s syscall sys_ppu_thread_rename(thread_id=0x%lx, name=%s)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, STR(r4));
			}
			else if(num == SYS_NET_SOCKET) {
				lv2_printf("%s syscall sys_net_bnet_socket(family=%ld, type=%ld, protocol=%ld)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5);
			}
			else if(num == SYS_NET_CLOSE) {
				/*				uint8_t *thread_info = (uint8_t*)get_current_thread();
				if(thread_info) {
				  lv2_printf("thread_info= ");
				  for(uint16_t i = 0; i < sizeof(thread_t); i++)
				  {
					 lv2_printf("%02x", *(thread_info + i));
				  }
				  lv2_printf("\n");
				}*/

				/*uint8_t *thread_info = (uint8_t*)get_current_thread();
				if(thread_info) {
				  char* buf = (char*)alloc(KB(64), 0x27);
				  if(buf) {
					char* cur = buf;
					for(uint16_t i = 0; i < sizeof(thread_s); i++)
					{
					   cur += lv2_sprintf(cur, "%02x", *(thread_info + i));
					}
					lv2_printf("thread_info= %s\n", buf);
					dealloc(buf, 0x27);
				  }
				}*/
				lv2_printf("%s syscall sys_net_bnet_close(s=%ld)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3);
			}
			else if(num == SYS_NET_SYSCTL) {
				lv2_printf("%s syscall sys_net_bnet_sysctl(0x%lx, 0x%lx, 0x%lx, *0x%lx, 0x%lx, 0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6, r7, r8);
			}
			else if(num == SYS_NET_SENDTO) {
				lv2_printf("%s syscall sys_net_bnet_sendto(s=%ld, buf=*0x%lx, len=%lu, flags=0x%lx, addr=*0x%lx, addrlen=%lu)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6, r7, r8);
			}
			else if(num == SYS_NET_CONTROL) {
				lv2_printf("%s syscall sys_net_control(0x%lx, %ld, *0x%lx, %ld)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6);
			}
			else if(num == SYS_CELLFSLSEEK) {
				lv2_printf("%s syscall sys_fs_lseek(fd=%ld, offset=0x%lx, whence=0x%lx, pos=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6);
			}
			else if(num == SYS_MEMORY_ALLOCATE_FROM_CONTAINER) {
				lv2_printf("%s syscall sys_memory_allocate_from_container(size=0x%lx, cid=0x%lx, flags=0x%lx, alloc_addr=*0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6);
			}
			else if(num == SYS_COND_WAIT) {
				lv2_printf("%s syscall sys_cond_wait(cond_id=0x%lx, timeout=%ld)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4);
			}
			else if(num == SYS_HID_MANAGER_IOCTL) {
				lv2_printf("%s syscall sys_hid_manager_ioctl(hid_handle=0x%lx, pkg_id=0x%lx, buf=*0x%lx, buf_size=0x%lx)\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", r3, r4, r5, r6);
			}
			else {
				lv2_printf("%s %lx %s syscall %ld %lx %lx %lx %lx %lx %lx %lx %lx\n", get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL", (uint64_t)get_current_thread(), get_current_thread_name(), num, r3, r4, r5, r6, r7, r8, r9, r10);
			}
		}
	}

	resume_intr();

	return syscall(r3, r4, r5, r6, r7, r8, r9, r10);
}


/*
 * Enables the hook
 */
void syscall_hook_init(void)
{
	set_syscall_handler(syscall_handler);
}
