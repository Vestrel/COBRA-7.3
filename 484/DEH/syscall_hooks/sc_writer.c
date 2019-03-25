#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/io.h>
#include <lv2/synchronization.h>
#include <lv2/memory.h>
#include <lv2/thread.h>
#include <lv2/error.h>

#include "utilities.h"
#include "sc_writer.h"
#include "sc_info.h"
#include "sc_pool.h"


/*
 * Defines
 */
#define SC_WRITER_FILE "/dev_hdd0/plugins/syscall.log"
#define SC_WRITER_QUEUE_SIZE 127


/*
 * Types
 */
typedef enum sc_write_type_e {
	SC_POOL_ELMNT = 0,
	SC_STRING
} sc_write_type_e;


/*
 * Variables
 */
static event_queue_t sc_evq;
static event_port_t sc_evp;

static thread_t sc_writer_thread;
static int sc_fd;
static int hdd0_mounted;


/*
 * File Handling
 */
static int sc_writer_open_log(void) {
	int err = cellFsOpen(SC_WRITER_FILE, CELL_FS_O_WRONLY | CELL_FS_O_CREAT | CELL_FS_O_TRUNC, &sc_fd, 0666, NULL, 0);
	if(err) {
		ERROR("Could not open/create syscall log file '" SC_WRITER_FILE "' (err=%x).", err);
		return -1;
	}
	if(!sc_fd) {
		ERROR("Could not open/create syscall log file '" SC_WRITER_FILE "' (fd==0).");
		return -2;
	}

	return 0;
}

#ifdef SC_HOOK_MOUNT
LV2_HOOKED_FUNCTION_PRECALL_SUCCESS_8(int, sc_writer_post_cellFsUtilMount, (const char *block_dev, const char *filesystem, const char *mount_point, int unk, int read_only, int unk2, char *argv[], int argc))
{
	/*INFO("cellFsUtilMount(blk_dev='%s', fs='%s', mount='%s', unk=%d, ro=%d, unk2=%d, argv=%lx, argc=%d)", block_dev, filesystem, mount_point, unk, read_only, unk2, (uint64_t)argv, argc);
	for(int i = 0; i < argc; i++) {
		INFO("\targv[i] = '%s'", argv[i]);
	}*/

	if (!hdd0_mounted && strcmp(mount_point, "/dev_hdd0") == 0 && strcmp(filesystem, "CELL_FS_UFS") == 0) {
		hdd0_mounted = 1;

		INFO("HDD0 mounted! Switching to log file...");
		sc_writer_open_log();
	}
	
	return 0;
}
#endif

void sc_write(char *buf, uint64_t size) {
	#ifndef SC_LOG_TO_FILE
		lv2_printf(buf);
		return;
	#else

		if(!sc_fd) {
			#ifdef SC_HOOK_MOUNT
				lv2_printf("SCH LV2: %s", buf);
				return;
			#else
				ERROR("sc_write->sc_fd==0! Stopping syscall logging.");
				ppu_thread_exit(1);
			#endif
		}

		if(size == 0)
			size = strlen(buf);

		int err = cellFsWrite(sc_fd, buf, size, NULL);

		if(err) {
			ERROR("sc_write->cellFsWrite failed (err=%x)! Stopping syscall logging.", err);
			cellFsClose(sc_fd);
			sc_fd = 0;
			ppu_thread_exit(1);
		}
	#endif
}


int sc_writer_dump_buffer(char *dst, int dst_len, void *src, int src_len) {
	// we need at least space for two hex digits plus the null terminator
	if(dst_len < 3)
		return 0;

	uint8_t *src8 = (uint8_t*)src;

	// If we overflow, reserve at least 3 characters
	int overflow = 0;
	int wr_len = src_len * 2 + 1;

	if(wr_len > dst_len) {
		overflow = 1;
		wr_len = dst_len;
	}

	int i;
	for(i = wr_len; i > 2; i -= 2) { \
		dst += lv2_sprintf(dst, "%02x", *src8);
		src8 += 1;
	}

	if(overflow) {
		if(i <= 1)
			dst -= 1;

		*(dst++) = '?';
		*(dst++) = '\0';
	}

	return wr_len - i;
}

/*
 * Handle syscalls
 */
void default_syscall_printer(sc_pool_elmnt_t *pe)
{
	syscall_info_t *info = pe->info;

	// Prepare buffer
	SCW_START

	// Prepare variables
	#ifndef SC_LOG_MINIMUM
		char *nm   = info->name;
		char *proc_nm = "?";
		char *thrd_nm = "?";

		sc_pe_next_str(pe, &proc_nm, NULL);
		sc_pe_next_str(pe, &thrd_nm, NULL);
	#endif

	// Print Header
	#ifdef SC_LOG_MINIMUM
		SCW_PRINTF("%u(%hhu)> [%x,%x] %hd ", pe->uid, pe->hwt, pe->pid, pe->tid, info->num);
	#else
		SCW_PRINTF("%u(%hhu)> [PID=%x '%s'; TID=%x '%s'] %hd %s(", pe->uid, pe->hwt, pe->pid, proc_nm, pe->tid, thrd_nm, info->num, nm);
	#endif
	
	// Print args
	uint16_t nargs = info->nargs;
	for(uint16_t i = 0; i < nargs; i++) {
		#ifdef SC_LOG_MINIMUM
			SCW_PRINTF("%lx ", pe->args[i]);
		#else
			if(i > 0)
				SCW_PRINTF(", ");

			if(info->arg_ptr[i]) {
				SCW_PRINTF(info->arg_fmt[i], pe->args[i], pe->args_ptd[i]);
			} else {
				SCW_PRINTF(info->arg_fmt[i], pe->args[i]);
			}
		#endif
	}

	// Print footer
	if(pe->has_res) {
		#ifdef SC_LOG_MINIMUM
			SCW_PRINTF("-> %lx\n", pe->res);
		#else
			SCW_PRINTF(") -> 0x%lx\n", pe->res);
		#endif
	}
	else {
		#ifdef SC_LOG_MINIMUM
			SCW_PRINTF("-> ?\n");
		#else
			SCW_PRINTF(") -> ?\n");
		#endif
	}

	SCW_FINISH
}

static INLINE void on_syscall(sc_pool_elmnt_t *pe) {
	#ifndef SC_WRITER_NO_CALLBACKS
		// Trigger pre-writer callback (if it exists)
		sc_writer_callback *cb = pe->info->pre_write_cb;
		if(cb && cb(pe) != 0)
			return;
	#endif

	// Default printer
	default_syscall_printer(pe);
	
	#ifndef SC_WRITER_NO_CALLBACKS
		// Trigger post-writer callback (if it exists)
		cb = pe->info->post_write_cb;
		if(cb)
			cb(pe);
	#endif
}


/*
 * Writer Thread
 */
static void sc_writer_thread_main(uint64_t arg) {
	INFO("SC Writer Thread initialized!\nSyscalls will be logged to file '" SC_WRITER_FILE "'.");

	for(;;) {
		event_t ev;

		// Wait for event
		int err = event_queue_receive(sc_evq, &ev, 0);
		if(err != 0) {
			ERROR("event_queue_receive returned error %x", err);
			ppu_thread_exit(2);
			return;
		}
		//INFO("queue data received");

		// Process write
		void *ptr = (void*)(ev.data1);
		sc_write_type_e type = (sc_write_type_e)(ev.data2);
		int do_free = (int)(ev.data3);

		if(type == SC_POOL_ELMNT) {
			// Process pool element
			sc_pool_elmnt_t *pe = (sc_pool_elmnt_t*)ptr;

			if(pe->in_use) {
				// Trace Buffer
				sc_pe_restart(pe);
				on_syscall(pe);

				// Unlock
				sc_pe_unlock(pe);
			}
			else {
				ERROR("sc_writer: pe->in_use=0!");
			}
		}
		else {
			// Write to file
			char *buf = (char*)ptr;

			sc_write(buf, 0);
		}

		// Free buffer
		if(do_free)
			free(ptr);
	}

	ppu_thread_exit(0);
}


/*
 * Initialization
 */
int init_syscall_writer(void) {
	// Event queue & port for inter-thread communication
	// TODO: Maybe implement a custom queue? This one can't be larger than 127...
	int err = event_queue_create(&sc_evq, SYNC_PRIORITY, /*event_queue_key*/ 1, /*size 1-127*/ SC_WRITER_QUEUE_SIZE );
	if(err != 0) {
		ERROR("Could not initialize sc_evq (err=%x)", err);
		return -1;
	}

	err = event_port_create(&sc_evp, EVENT_PORT_REMOTE);
	if(err != 0) {
		ERROR("Could not initialize sc_evp (err=%x)", err);
		return -2;
	}

	err = event_port_connect(sc_evp, sc_evq);
	if(err != 0) {
		ERROR("Could not connect sc_evp to sc_evq (err=%x)", err);
		return -3;
	}

	// Initialize log file
	sc_fd = 0;
	hdd0_mounted = 0;
	#ifndef SC_HOOK_MOUNT
		#ifdef SC_LOG_TO_FILE 
			if(!sc_writer_open_log()) {
				return -4;
			}
		#endif
	#else
		// Unhook cellFsUtilMount (restore original instruction at cellFsUtilMount_symbol) and then re-hook
		*(uint32_t *)MKA(cellFsUtilMount_symbol) = 0xF821FED1;
		hook_function_on_precall_success(cellFsUtilMount_symbol, sc_writer_post_cellFsUtilMount, 8);
	#endif

	// Create writer thread
	err = ppu_thread_create(&sc_writer_thread, &sc_writer_thread_main, /*arg*/ 0, /*prio*/ 0, /*stacksize*/ 0x4000, 0, "SCH_sc_writer");
	if(err != 0) {
		ERROR("Could not create sc_writer_thread (err=%x)", err);
		return -6;
	}

	// Success
	return 0;
}


/*
 * Utility Methods
 */
void sc_send_string(char *buf, int do_free) {
	int err = event_port_send(sc_evp, (uint64_t)buf, (uint64_t)SC_STRING, (uint64_t)do_free);

	if(err != 0
#ifndef SC_POOL_LOCKING
		&& err != EBUSY
#endif
	)
		ERROR("Could not submit to event_port (err=%x)", err);
}

void sc_send_pool_elmnt(sc_pool_elmnt_t *pe) {
	int err = event_port_send(sc_evp, (uint64_t)pe, (uint64_t)SC_POOL_ELMNT, 0);

	if(err != 0
#ifndef SC_POOL_LOCKING
		&& err != EBUSY
#endif
	) {
		ERROR("Could not submit to event_port (err=%x)", err);
		sc_pe_unlock(pe);
	}
}
