#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/io.h>
#include <lv2/synchronization.h>
#include <lv2/memory.h>
#include <lv2/thread.h>

#include "utilities.h"
#include "sc_writer.h"


/*
 * Defines
 */
#define SC_POOL_LOCKING
#define SC_POOL_ELEMNT_COUNT 127


/*
 * Types
 */
typedef struct sc_pool_t {
	sc_pool_elmnt_t elemnts[SC_POOL_ELEMNT_COUNT];
} sc_pool_t;


/*
 * Variables
 */
static sc_pool_t *sc_pool;
uint32_t uid_counter;

#ifdef SC_POOL_LOCKING
	static sema_t sc_pool_sem;
#endif


/*
 * Initialization
 */
int init_syscall_pool(void) {
	// Buffer pool
	sc_pool = (sc_pool_t*)malloc(sizeof(sc_pool_t));
	if(sc_pool == NULL) {
		ERROR("Could not allocate sc_pool.");
		return -1;
	}
	memset(sc_pool, 0, sizeof(sc_pool_t));
	
	for(uint16_t i = 0; i < SC_POOL_ELEMNT_COUNT; i++) {
		void *buf = malloc(SC_POOL_BUF_SIZE);

		if(buf == NULL) {
			ERROR("Could not allocate buffer for sc_pool_elmtn i=%hu", i);
			return -2;
		}

		sc_pool->elemnts[i].buf = buf;
	}

	#ifdef SC_POOL_LOCKING
		// Semaphore
		semaphore_initialize(&sc_pool_sem, SYNC_PRIORITY, SC_POOL_ELEMNT_COUNT, SC_POOL_ELEMNT_COUNT, SYNC_NOT_PROCESS_SHARED);
	#endif

	// Success
	INFO("SC Buffer Pool initialized!");
	return 0;
}


/*
 * Utility Methods
 */
void sc_pe_unlock(sc_pool_elmnt_t *pe) {
	pe->in_use = 0;

	#ifdef SC_POOL_LOCKING
		semaphore_post(&sc_pool_sem, 1);
	#endif
}

void sc_pe_restart(sc_pool_elmnt_t *pe) {
	pe->buf_pos = pe->buf;
}

#define PE_CHECK_SIZE(pe, err) do { \
		if(pe->buf_pos > pe->buf_end) \
			return err; \
	} while(0)

#define PE_CHECK_ERROR(cond) do { \
		sc_pe_ret_e __ret = (cond); \
		if(__ret != SC_PE_OK) return __ret; \
	} while(0)

#define DECL_PE_INT(suffix, type) \
sc_pe_ret_e sc_pe_next_##suffix(sc_pool_elmnt_t *pe, type *out) { \
	PE_CHECK_SIZE(pe, SC_PE_END); \
\
	type *ptr = (type*)(pe->buf_pos); \
\
	pe->buf_pos += sizeof(type); \
	PE_CHECK_SIZE(pe, SC_PE_OVERFLOW); \
\
	*out = *ptr; \
\
	return SC_PE_OK; \
} \
\
sc_pe_ret_e sc_pe_add_##suffix(sc_pool_elmnt_t *pe, type in) { \
	type *ptr = (type*)(pe->buf_pos); \
\
	pe->buf_pos += sizeof(in); \
	if(pe->buf_pos > pe->buf + SC_POOL_BUF_SIZE) \
		return SC_PE_OVERFLOW; \
\
	*ptr = in; \
	pe->buf_end = pe->buf_pos; \
\
	return SC_PE_OK; \
}

DECL_PE_INT(u8 , uint8_t );
DECL_PE_INT(u16, uint16_t);
DECL_PE_INT(u32, uint32_t);
DECL_PE_INT(u64, uint64_t);
DECL_PE_INT(i8 , int8_t );
DECL_PE_INT(i16, int16_t);
DECL_PE_INT(i32, int32_t);
DECL_PE_INT(i64, int64_t);

void* sc_pe_get_next_pos(sc_pool_elmnt_t *pe) {
	return (void*)(pe->buf_pos + sizeof(uint16_t));
}

sc_pe_ret_e sc_pe_next(sc_pool_elmnt_t *pe, void **out, uint16_t *len) {
	uint16_t _len;
	PE_CHECK_ERROR(sc_pe_next_u16(pe, &_len));

	if(len != NULL) *len = _len;
	if(out != NULL) *out = _len ? (void*)(pe->buf_pos) : NULL;
	pe->buf_pos += _len;

	PE_CHECK_SIZE(pe, SC_PE_OVERFLOW);

	return SC_PE_OK;
}

sc_pe_ret_e sc_pe_next_str(sc_pool_elmnt_t *pe, char **out, uint16_t *len) {
	return sc_pe_next(pe, (void**)out, len);
}

sc_pe_ret_e sc_pe_add(sc_pool_elmnt_t *pe, void *in, uint16_t len) {
	void *ptr = pe->buf_pos;

	pe->buf_pos += len + sizeof(uint16_t);
	if(pe->buf_pos > pe->buf + SC_POOL_BUF_SIZE)
		return SC_PE_OVERFLOW;

	*(uint16_t*)ptr = len;

	if(len > 0)
		memcpy(ptr + sizeof(uint16_t), in, len);

	pe->buf_end = pe->buf_pos;

	return SC_PE_OK;
}

sc_pe_ret_e sc_pe_add_str(sc_pool_elmnt_t *pe, char *in) {
	return sc_pe_add(pe, (void*)in, in ? strlen(in)+1 : 0);
}

uint16_t sc_pe_remaining(sc_pool_elmnt_t *pe) {
	if(pe->buf_pos >= pe->buf_end) return 0;
	return (uint16_t)(pe->buf_end - pe->buf_pos);
}

uint16_t sc_pe_remaining_space(sc_pool_elmnt_t *pe) {
	void *end = pe->buf + SC_POOL_BUF_SIZE;
	if(pe->buf_pos >= end) return 0;
	return (uint16_t)(end - pe->buf_pos);
}



sc_pool_elmnt_t* sc_pool_get_elmnt(void) {
	// Calculate unique ID
	uint32_t uid, tmp;
	__asm__ __volatile__ (
		"retry_uid: \n\t"
		"lwarx %[dst], 0, %[addr] \n\t"
		"addi %[tmp], %[dst], 1 \n\t"
		"stwcx. %[tmp], 0, %[addr] \n\t"
		"bne- retry_uid \n\t"
		: [dst] "=&r" (uid), [tmp] "=&r" (tmp)
		: [addr] "r" (&uid_counter)
	);

	#ifdef SC_POOL_LOCKING
		// Wait until one buffer is free
		semaphore_wait(&sc_pool_sem, 0);
	#endif

	// Search for free buffer slot
	sc_pool_elmnt_t *pe = sc_pool->elemnts;

	uint32_t i;
	for(i = 0; i < SC_POOL_ELEMNT_COUNT; i++, pe++) {
		uint32_t *in_use_ptr = &(pe->in_use);
		uint32_t in_use = 0;

		// exchange 'in_use' atomically by 1
		__asm__ __volatile__ (
			"retry_in_use: \n\t"
			"lwarx %[dst], 0, %[addr] \n\t"
			"stwcx. %[one], 0, %[addr] \n\t"
			"bne- retry_in_use \n\t"
			: [dst] "=&r" (in_use)
			: [addr] "r" (in_use_ptr), [one] "r" (1)
		);

		if(!in_use)
			break;
	}

	// Sanity check
	if(i >= SC_POOL_ELEMNT_COUNT) {
		#ifdef SC_POOL_LOCKING
			ERROR("sc_pool_get_elmnt: Out of Buffers!");
		#endif
		return NULL;
	}
	
	// Reset PE and return
	pe->uid = uid;
	pe->buf_pos = pe->buf;
	pe->buf_end = pe->buf;
	return pe;
}
