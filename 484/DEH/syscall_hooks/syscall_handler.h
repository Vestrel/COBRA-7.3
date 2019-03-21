#ifndef __SYSCALL_HANDLER_H__
#define __SYSCALL_HANDLER_H__

#include <lv2/syscall.h>
#include <lv2/thread.h>

struct syscall_t;
#include "syscall_info.h"


/*
 * Defines
 */
#define _SC_PRINTF(buf, len, fmt, ...) do { \
	uint64_t __wrlen = (uint64_t)snprintf(buf, len, fmt, ## __VA_ARGS__ ); \
	if(__wrlen >= len) { \
		WARN("_SC_PRINTF - buffer size=%lx too small, needed %lx", (uint64_t)len, __wrlen+1); \
	} \
} while(0)

#define SC_PRINTF(max_len, fmt, ...) do { \
	char *__sc_buf = (char*)malloc(max_len * sizeof(char)); \
	if(__sc_buf == NULL) { \
		ERROR("SC_PRINTF - could not allocate buffer"); \
	} \
	else { \
		_SC_PRINTF(__sc_buf, max_len, fmt, ## __VA_ARGS__ ); \
		sc_send_and_free_buffer(__sc_buf); \
	} \
} while(0)


/*
 * Types
 */

// Structure describing the current syscall being processed
typedef struct syscall_t {
	char     *proc;
	thread_s *thr;
	struct syscall_info_t *info;

	uint64_t  num;
	uint64_t  res;
	uint64_t  args[8];
} syscall_t;


/*
 * Methods
 */
extern void default_syscall_printer(syscall_t *sc);
extern int init_syscall_handler(void);


#endif // __SYSCALL_HANDLER_H__
