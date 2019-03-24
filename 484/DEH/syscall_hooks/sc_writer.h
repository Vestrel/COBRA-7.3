#ifndef _SC_WRITER_H_
#define _SC_WRITER_H_

#include "sc_pool.h"

/*
 * Defines
 */

// Length of the scratch buffer
#define SCW_SCRATCH_BUF_LEN 2048

// Define to use a single accumulative buffer - faster (less I/O ops), but may truncate output
#define USE_ACC_BUF

// Minimum log size - will only trace the most important things
//#define SC_LOG_MINIMUM

// Log to file
#define SC_LOG_TO_FILE

// Hook cellFsUtilMount to detect hdd0 being mounted
#define SC_HOOK_MOUNT

// Disable writer callback
//#define SC_WRITER_NO_CALLBACKS

// Helper macros
#define SCW_IND_SNPRINTF(buf, len, fmt, ...) do { \
	uint64_t __wrlen = (uint64_t)snprintf(buf, len, fmt, ## __VA_ARGS__ ); \
	if(__wrlen >= len) { \
		WARN("SC_SNPRINTF - buffer size=%lx too small, needed %lx", (uint64_t)len, __wrlen+1); \
		__wrlen = len-1; \
	} \
	sc_write(buf, __wrlen); \
} while(0)

#define SCW_IND_NPRINTF(max_len, fmt, ...) do { \
	char __sc_buf[max_len]; \
	SC_IND_SNPRINTF(__sc_buf, max_len, fmt, ## __VA_ARGS__ ); \
} while(0)

#define SCW_ACC_PRINTF(buf, buf_len, cur_len, fmt, ...) do { \
	int __remaining = buf_len - cur_len - 1; \
	if(__remaining > 0) { \
		char *__cur = buf + cur_len; \
		cur_len += snprintf(__cur, __remaining+1, fmt, ## __VA_ARGS__ ); \
		if(cur_len >= buf_len) \
			cur_len = buf_len-1; \
	} \
} while(0)

#define _SCW_DUMP(dst, src, len) do { \
		uint8_t *__mem8 = (uint8_t*)(src); \
		char *__dst = (char*)(dst); \
		\
		uint32_t __i; \
		for(__i = 0; __i < len; __i++) { \
			__dst += lv2_sprintf(__dst, "%02x", *__mem8); \
			__mem8 += 1; \
		} \
		scw_len += __i * 2; \
	} while(0)


#ifdef USE_ACC_BUF
	#define SCW_START \
		char scw_scratch[SCW_SCRATCH_BUF_LEN]; \
		int scw_len = 0;
	#define SCW_PRINTF(fmt, ...) SCW_ACC_PRINTF(scw_scratch, SCW_SCRATCH_BUF_LEN, scw_len, fmt, ##__VA_ARGS__ )
	#define SCW_DUMP(src, len) _SCW_DUMP(scw_scratch + scw_len, src, len)
	#define SCW_FINISH sc_write(scw_scratch, 0);
#else
	#define SCW_START \
		char scw_scratch[SCW_SCRATCH_BUF_LEN];
	#define SCW_PRINTF(fmt, ...) SCW_IND_SNPRINTF(scw_scratch, SCW_SCRATCH_BUF_LEN, fmt, ##__VA_ARGS__ )
	#define SCW_DUMP(src, len) _SCW_DUMP(scw_scratch, src, len)
	#define SCW_FINISH
#endif

/*
 * Methods
 */
extern int init_syscall_writer(void);
//extern void default_syscall_printer(syscall_t *sc);

extern void sc_write(char *buf, uint64_t size);

extern void sc_send_string(char *buf, int do_free);
extern void sc_send_pool_elmnt(sc_pool_elmnt_t *sc_pe);



#endif // _SC_WRITER_H_
