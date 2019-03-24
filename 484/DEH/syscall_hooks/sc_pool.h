#ifndef _SC_POOL_H_
#define _SC_POOL_H_

typedef struct sc_pool_elmnt_t sc_pool_elmnt_t;
#include "sc_writer.h"

/*
 * Defines
 */
#define SC_POOL_BUF_SIZE KB(4)
#define SYSCALLT_MAX_NAME_LEN 32
#define SC_POOL_LOCKING


/*
 * Types
 */

// Pool element
struct sc_pool_elmnt_t {
	uint32_t uid; // Unique ID
	uint32_t in_use; // 0 if this buffer is available for use, 1 otherwise

	struct syscall_info_t *info;

	uint64_t args[8];
	uint64_t res;

	uint32_t pid;
	uint32_t tid;
	uint8_t hwt;

	uint8_t has_res;

	void *buf; // pointer to a buffer of SC_POOL_BUF_SIZE
	void *buf_pos; // current position inside 'buf'
	void *buf_end;
};

typedef enum sc_pe_ret_e {
	SC_PE_OK = 0,
	SC_PE_END,
	SC_PE_OVERFLOW
} sc_pe_ret_e;


/*
 * Methods
 */
extern int init_syscall_pool(void);
//extern void default_syscall_printer(syscall_t *sc);

extern sc_pool_elmnt_t* sc_pool_get_elmnt(void);

extern void sc_pe_unlock(sc_pool_elmnt_t *pe);
extern void sc_pe_restart(sc_pool_elmnt_t *pe); // restart from the beginning of 'buf'

#define HEADER_PE_INT(suffix, type) \
extern sc_pe_ret_e sc_pe_next_##suffix(sc_pool_elmnt_t *pe, type *out); \
extern sc_pe_ret_e sc_pe_add_##suffix(sc_pool_elmnt_t *pe, type in);

HEADER_PE_INT(u8 , uint8_t );
HEADER_PE_INT(u16, uint16_t);
HEADER_PE_INT(u32, uint32_t);
HEADER_PE_INT(u64, uint64_t);
HEADER_PE_INT(i8 , int8_t );
HEADER_PE_INT(i16, int16_t);
HEADER_PE_INT(i32, int32_t);
HEADER_PE_INT(i64, int64_t);
#undef HEADER_PE_INT

extern void* sc_pe_get_next_pos(sc_pool_elmnt_t *pe);

extern sc_pe_ret_e sc_pe_next(sc_pool_elmnt_t *pe, void **out, uint16_t *len);
extern sc_pe_ret_e sc_pe_add(sc_pool_elmnt_t *pe, void *in, uint16_t len);

extern sc_pe_ret_e sc_pe_next_str(sc_pool_elmnt_t *pe, char **out, uint16_t *len);
extern sc_pe_ret_e sc_pe_add_str(sc_pool_elmnt_t *pe, char *in);

extern uint16_t sc_pe_remaining(sc_pool_elmnt_t *pe);
extern uint16_t sc_pe_remaining_space(sc_pool_elmnt_t *pe);

#define SC_PE_NEXT(pe, type, out, len) do { \
		void *__ptr = (void*)out; \
		sc_pe_next(pe, &__ptr, len); \
		out = (type*)__ptr; \
	} while(0)



#endif // _SC_POOL_H_
