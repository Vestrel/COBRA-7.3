#ifndef _SC_POOL_H_
#define _SC_POOL_H_

typedef struct sc_pool_elmnt_t sc_pool_elmnt_t;
#include "sc_writer.h"

/*
 * Defines
 */
#define SC_POOL_BUF_SIZE KB(4)
#define SYSCALLT_MAX_NAME_LEN 32


/*
 * Types
 */

// Pool element
struct sc_pool_elmnt_t {
	uint64_t owner; // unique ID of owner of this object, e.g. the thread that did the syscall
	uint32_t in_use; // 0 if this buffer is available for use, 1 otherwise

	struct syscall_info_t *info;

	uint64_t args[8];
	uint64_t res;
	uint8_t has_res;

	void *buf; // pointer to a buffer of SC_POOL_BUF_SIZE

	int   buf_len; // length of 'buf'
	void *buf_ptr; // current position inside 'buf'
};


/*
 * Methods
 */
extern int init_syscall_pool(void);
//extern void default_syscall_printer(syscall_t *sc);

extern sc_pool_elmnt_t* sc_pool_get_elmnt(void);

extern void sc_pe_restart(sc_pool_elmnt_t *pe); // restart from the beginning of 'buf'
//extern void sc_pe_next(sc_pool_elmnt_t *pe);
//extern void* sc_pool_buf_get_next(int *size);
//extern char* sc_pool_buf_get_string(int *len);



#endif // _SC_POOL_H_
