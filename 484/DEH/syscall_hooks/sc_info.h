#ifndef __SYSCALL_INFO_H__
#define __SYSCALL_INFO_H__

#include <lv2/syscall.h>

struct syscall_info_t;
#include "sc_pool.h"

/*
 * Syscall Info structure
 */
#define SCI_ARGS_MAX 8

/* Types */

// Callback type for custom dumping
// If it returns 0, the default dumper is still executed
typedef int (sc_prepare_callback)(struct sc_pool_elmnt_t *pe);
typedef int (sc_writer_callback)(struct sc_pool_elmnt_t *pe);

typedef struct syscall_info_t {
	uint16_t num;
	uint8_t nargs;

	uint8_t trace; // Whether to trace this syscall
	
	char *name;

	sc_prepare_callback *prepare_cb; // Callback for setting up the sc_pool_elmnt_t
	sc_writer_callback *writer_cb; // Callback for custom dumping
	
	char* arg_fmt[SCI_ARGS_MAX]; // Information about the various parameters
} syscall_info_t;

extern syscall_info_t *syscall_info;


/* Methods */
extern int init_syscall_info(void);
extern syscall_info_t* get_syscall_info(uint16_t num);

#endif
