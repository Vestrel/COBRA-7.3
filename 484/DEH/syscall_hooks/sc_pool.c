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
#define SC_POOL_ELEMNT_COUNT 256


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

	// Success
	INFO("SC Buffer Pool initialized!");
	return 0;
}


/*
 * Utility Methods
 */
sc_pool_elmnt_t* sc_pool_get_elmnt(void) {
	// Search for free buffer slot
	sc_pool_elmnt_t *pe = sc_pool->elemnts;
	char *err = NULL;

	uint32_t i;
	for(i = 0; i < SC_POOL_ELEMNT_COUNT; i++) {
		uint32_t *in_use_ptr = &(pe[i].in_use);
		uint32_t in_use = 0;

		// exchange 'in_use' atomically by 1
		__asm__ __volatile__ (
			"retry: \n\t"
			"lwarx %[dst], 0, %[addr] \n\t"
			"stwcx. %[one], 0, %[addr] \n\t"
			"bne- retry \n\t"
			: [dst] "=&r" (in_use)
			: [addr] "r" (in_use_ptr), [one] "r" (1)
		);

		if(!in_use)
			break;
	}

	// Reserve/allocate buffer
	if(i >= SC_POOL_ELEMNT_COUNT) {
		pe = NULL;
		err = ">OOB<\n";
	}

	// Return
	if(err != NULL) {
		sc_send_string(err, 0);
		//ERROR("%s", err);
		return NULL;
	}

	// Point 'cur' pointer to beginning of 'buf', and return
	pe->buf_len = 0;
	pe->buf_ptr = pe->buf;
	return pe;
}


void sc_pe_restart(sc_pool_elmnt_t *pe) {
	pe->buf_ptr = pe->buf;
}
