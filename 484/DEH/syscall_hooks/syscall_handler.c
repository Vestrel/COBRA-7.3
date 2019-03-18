#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syscall_handler.h"
#include "utilities.h"

/*
 * Default printer
 */
void default_syscall_printer(syscall_t *sc)
{
	// Extend kstack to guarantee at least 4096 bytes of leftover stack
	extend_kstack(0);

	syscall_info_t *info = sc->info;

	// Parameters
	char buf[2048];
	char *cur = buf;
	uint16_t nargs = info->nargs;
	for(uint16_t i = 0; i < nargs; i++) {
		if(i > 0) {
			*(cur++) = ',';
			*(cur++) = ' ';
		}

		cur += sprintf(info->args[i].fmt, (char*)sc->args[i]);
	}

	// Actually print
	char *nm     = info->name;
	char *proc   = sc->proc;
	uint64_t thr = (uint64_t)sc->thr;
	PRINTF("%s (t=%lx) > %s(%s) -> %lx", proc, thr, nm, buf, sc->res);
}


void legacy_syscall_printer(syscall_t *sc)
{
	uint64_t r3   = sc->args[0];
	uint64_t r4   = sc->args[1];
	uint64_t r5   = sc->args[2];
	uint64_t r6   = sc->args[3];
	uint64_t r7   = sc->args[4];
	uint64_t r8   = sc->args[5];
	uint64_t r9   = sc->args[6];
	uint64_t r10  = sc->args[7];
	uint64_t num  = sc->num;
	uint64_t res  = sc->res;
	char*    proc = sc->proc;
	uint64_t thr  = (uint64_t)sc->thr;

	if(num == SYS_HID_MANAGER_CHECK_FOCUS) {
		PRINTF("%s syscall sys_hid_manager_check_focus() -> 0x%lx\n", proc, res);
	}
	else if(num == SYS_COND_SIGNAL) {
		PRINTF("%s syscall sys_cond_signal(cond_id=0x%lx) -> 0x%lx\n", proc, r3, res);
	}
	else if(num == SYS_MEMORY_ALLOCATE) {
		PRINTF("%s syscall sys_memory_allocate(size=0x%lx, flags=0x%lx, alloc_addr=*0x%lx) -> 0x%lx\n", proc, r3, r4, r5, res);
	}
	else if(num == SYS_LWMUTEX_LOCK) {
		PRINTF("%s syscall sys_lwmutex_lock(lwmutex=*0x%lx, timeout=0x%lx) -> 0x%lx\n", proc, r3, r4, res);
	}
	else if(num == SYS_LWMUTEX_UNLOCK) {
		PRINTF("%s syscall sys_lwmutex_unlock(lwmutex=*0x%lx) -> 0x%lx\n", proc, r3, res);
	}
	else if(num == SYS_MEMORY_GET_PAGE_ATTRIBUTE) {
		PRINTF("%s syscall sys_memory_get_page_attribute(addr=0x%lx, attr=*0x%lx) -> 0x%lx\n", proc, r3, r4, res);
	}
	else if(num == SYS_NET_INFOCTL) {
		PRINTF("%s syscall sys_net_infoctl(cmd=%ld, args=*0x%lx) -> 0x%lx\n", proc, r3, r4, res);
	}
	else if(num == SYS_NET_ABORT) {
		PRINTF("%s syscall sys_net_abort(type=%ld, arg=0x%lx, flags=0x%lx) -> 0x%lx\n", proc, r3, r4, r5, res);
	}
	else if(num == SYS_NET_IOCTL) {
		PRINTF("%s syscall sys_net_bnet_ioctl(socket_id=%ld, command=0x%lx, args=0x%lx) -> 0x%lx\n", proc, r3, r4, r5, res);
	}
	else if(num == SYS_HID_MANAGER_514) {
		PRINTF("%s syscall sys_hid_manager_514(%lx %lx %lx) -> 0x%lx\n", proc, r3, r4, r5, res);
	}
	else if(num == _SYS_PRX_START_MODULE) {
		PRINTF("%s syscall sys_prx_start_module(id=0x%lx, args=%lu, argp=*0x%lx, result=*0x%lx, flags=0x%lx, pOpt=*0x%lx) -> 0x%lx\n", proc, r3, r4, r5, r6, r7, r8, res);
	}
	else if(num == SYS_HID_MANAGER_READ) {
		PRINTF("%s syscall sys_hid_manager_read(handle=0x%lx, pkg_id=0x%lx, buf=*0x%lx, buf_size=0x%lx) -> 0x%lx\n", proc, r3, r4, r5, r6, res);
	}
	else if(num == SYS_LWCOND_SIGNAL) {
		PRINTF("%s syscall sys_lwcond_signal(lwcond=*0x%lx) -> 0x%lx\n", proc, r3, res);
	}
	else if(num == SYS_EVENT_QUEUE_DRAIN) {
		PRINTF("%s syscall sys_event_queue_drain(equeue_id=0x%lx) -> 0x%lx\n", proc, r3, res);
	}
	else if(num == SYS_NET_SHUTDOWN) {
		PRINTF("%s syscall sys_net_bnet_shutdown(s=%ld, how=%ld) -> 0x%lx\n", proc, r3, r4, res);
	}
	else if(num == SYS_PPU_THREAD_CREATE) {
		PRINTF("%s syscall _sys_ppu_thread_create(thread_id=*0x%lx, param=*0x%lx, arg=0x%lx, unk=0x%lx, prio=%ld, stacksize=0x%lx, flags=0x%lx, threadname=%s) -> 0x%lx\n", proc, r3, r4, r5, r6, r7, r8, r9, STR(r10), res);
	}
	else if(num == SYS_PPU_THREAD_RENAME) {
		PRINTF("%s syscall sys_ppu_thread_rename(thread_id=0x%lx, name=%s) -> 0x%lx\n", proc, r3, STR(r4), res);
	}
	else if(num == SYS_NET_SOCKET) {
		PRINTF("%s syscall sys_net_bnet_socket(family=%ld, type=%ld, protocol=%ld) -> 0x%lx\n", proc, r3, r4, r5, res);
	}
	else if(num == SYS_NET_CLOSE) {
		PRINTF("%s syscall sys_net_bnet_close(s=%ld) -> 0x%lx\n", proc, r3, res);
	}
	else if(num == SYS_NET_SYSCTL) {
		PRINTF("%s syscall sys_net_bnet_sysctl(0x%lx, 0x%lx, 0x%lx, *0x%lx, 0x%lx, 0x%lx) -> 0x%lx\n", proc, r3, r4, r5, r6, r7, r8, res);
	}
	else if(num == SYS_NET_SENDTO) {
		PRINTF("%s syscall sys_net_bnet_sendto(s=%ld, buf=*0x%lx, len=%lu, flags=0x%lx, addr=*0x%lx, addrlen=%lu) -> 0x%lx\n", proc, r3, r4, r5, r6, r7, r8, res);
	}
	else if(num == SYS_NET_CONTROL) {
		PRINTF("%s syscall sys_net_control(0x%lx, %ld, *0x%lx, %ld) -> 0x%lx\n", proc, r3, r4, r5, r6, res);
	}
	else if(num == SYS_CELLFSLSEEK) {
		PRINTF("%s syscall sys_fs_lseek(fd=%ld, offset=0x%lx, whence=0x%lx, pos=*0x%lx) -> 0x%lx\n", proc, r3, r4, r5, r6, res);
	}
	else if(num == SYS_MEMORY_ALLOCATE_FROM_CONTAINER) {
		PRINTF("%s syscall sys_memory_allocate_from_container(size=0x%lx, cid=0x%lx, flags=0x%lx, alloc_addr=*0x%lx) -> 0x%lx\n", proc, r3, r4, r5, r6, res);
	}
	else if(num == SYS_COND_WAIT) {
		PRINTF("%s syscall sys_cond_wait(cond_id=0x%lx, timeout=%ld) -> 0x%lx\n", proc, r3, r4, res);
	}
	else if(num == SYS_HID_MANAGER_IOCTL) {
		PRINTF("%s syscall sys_hid_manager_ioctl(hid_handle=0x%lx, pkg_id=0x%lx, buf=*0x%lx, buf_size=0x%lx) -> 0x%lx\n", proc, r3, r4, r5, r6, res);
	}
	else {
		PRINTF("%s %lx syscall %ld %lx %lx %lx %lx %lx %lx %lx %lx -> %lx\n", proc, thr, num, r3, r4, r5, r6, r7, r8, r9, r10, res);
	}
}



/*
 * Syscall handler
 */
static void on_syscall(syscall_t *sc)
{
	// Prepare missing information
	sc->proc = get_current_process() ? get_process_name(get_current_process())+8 : "KERNEL";
	sc->thr  = get_current_thread();
	sc->info = &(syscall_info[sc->num]);

	// Trigger callback (if exists)
	sci_callback *cb = sc->info->cb;
	if(cb && cb(sc))
		return;

	// Default printer
	default_syscall_printer(sc);
	
	return;
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
	uint64_t (* syscall)() = (void *)&func;


	// Call original syscall and store result
	uint64_t res = syscall(r3, r4, r5, r6, r7, r8, r9, r10);

	// Simple filter for most spammy syscalls
	if (
		num == SYS_SM_GET_EXT_EVENT2 ||
		num == SYS_SPU_THREAD_GROUP_START ||
		num == SYS_SPU_THREAD_GROUP_JOIN ||
		num == SYS_EVENT_QUEUE_RECEIVE ||
		num == SYS_EVENT_PORT_SEND ||
		num == SYS_MUTEX_UNLOCK ||
		num == SYS_MUTEX_LOCK ||
		num == 579 /*sys_bluetooth_aud_serial_unk1*/ ||
		num == SYS_RWLOCK_RLOCK ||
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

	// Pause syscall
	suspend_intr();

	// Prepare a single memory representation of this structure, to avoid copying around data all the time
	if(num < MAX_NUM_OF_SYSTEM_CALLS) {
		syscall_t sc;

		sc.num = num;
		sc.res = res;
		sc.args[0] = r3;
		sc.args[1] = r4;
		sc.args[2] = r5;
		sc.args[3] = r6;
		sc.args[4] = r7;
		sc.args[5] = r8;
		sc.args[6] = r9;
		sc.args[7] = r8;

		// Dump syscall
		on_syscall(&sc);
		
		res = sc.res;
	}
	else
	{
		PRINTF("ERROR: syscall_handler with num=%lx >= MAX_NUM_OF_SYSTEM_CALLS", num);
	}

	// End syscall
	resume_intr();

	return res;
}



// Init
void init_syscall_handler(void)
{
	set_syscall_handler(syscall_handler);
}
