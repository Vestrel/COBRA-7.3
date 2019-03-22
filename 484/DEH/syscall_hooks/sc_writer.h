#ifndef _SC_WRITER_H_
#define _SC_WRITER_H_

#include "sc_pool.h"

/*
 * Defines
 */

// Length of the scratch buffer for the default printer
#define SCRATCH_BUF_LEN 2048

// Define to use a single accumulative buffer - faster (less I/O ops), but may truncate output
#define USE_ACC_BUF

#define SC_IND_SNPRINTF(buf, len, fmt, ...) do { \
	uint64_t __wrlen = (uint64_t)snprintf(buf, len, fmt, ## __VA_ARGS__ ); \
	if(__wrlen >= len) { \
		WARN("SC_SNPRINTF - buffer size=%lx too small, needed %lx", (uint64_t)len, __wrlen+1); \
		__wrlen = len-1; \
	} \
	sc_write(buf, __wrlen); \
} while(0)

#define SC_IND_NPRINTF(max_len, fmt, ...) do { \
	char __sc_buf[max_len]; \
	SC_IND_SNPRINTF(__sc_buf, max_len, fmt, ## __VA_ARGS__ ); \
} while(0)

#define SC_ACC_PRINTF(buf, buf_len, cur_len, fmt, ...) { \
	int __remaining = buf_len - cur_len - 1; \
	if(__remaining > 0) { \
		char *__cur = buf + cur_len; \
		cur_len += snprintf(__cur, __remaining+1, fmt, ## __VA_ARGS__ ); \
		if(cur_len >= buf_len) \
			cur_len = buf_len-1; \
	} \
} while(0)

/*
 * Methods
 */
extern int init_syscall_writer(void);
//extern void default_syscall_printer(syscall_t *sc);

extern void sc_write(char *buf, uint64_t size);

extern void sc_send_string(char *buf, int do_free);
extern void sc_send_pool_elmnt(sc_pool_elmnt_t *sc_pe);



#endif // _SC_WRITER_H_
