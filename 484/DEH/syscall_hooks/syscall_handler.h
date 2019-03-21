#ifndef __SYSCALL_HANDLER_H__
#define __SYSCALL_HANDLER_H__

#include <lv2/syscall.h>
#include <lv2/thread.h>

struct syscall_t;
#include "syscall_info.h"

/* Defines */
#define SC_SNPRINTF(buf, len, fmt, ...) do { \
	uint64_t __wrlen = (uint64_t)snprintf(buf, len, fmt, ## __VA_ARGS__ ); \
	if(__wrlen >= len) { \
		WARN("SC_SNPRINTF - buffer size=%lx too small, needed %lx", (uint64_t)len, __wrlen+1); \
		__wrlen = len-1; \
	} \
	sc_log(buf, __wrlen); \
} while(0)

#define SC_NPRINTF(max_len, fmt, ...) do { \
	char __sc_buf[max_len]; \
	SC_SNPRINTF(__sc_buf, max_len, fmt, ## __VA_ARGS__ ); \
} while(0)

/* Types */

// Structure describing the current syscall being processed
typedef struct syscall_t {
	char     *proc;
	thread_s *thr;
	struct syscall_info_t *info;

	uint64_t  num;
	uint64_t  res;
	uint64_t  args[8];
} syscall_t;

/* Variables */
extern int sch_output_fd;

/* Methods */
extern void sc_log(char *buf, uint64_t size);
extern void default_syscall_printer(syscall_t *sc);
extern int init_syscall_handler(void);


#include "syscall_info.h"

#endif // __SYSCALL_HANDLER_H__
