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

// Syscall groups
typedef struct syscall_group_t {
	uint16_t id;
	char *name;
} syscall_group_t;

// Callback type for custom dumping
// If it returns 0, the default dumper is still executed
typedef int (sc_callback)(struct sc_pool_elmnt_t *pe);

typedef struct syscall_info_t {
	uint16_t num;
	uint8_t nargs;

	uint8_t trace; // Whether to trace this syscall
	
	char *name;

	syscall_group_t *group;

	// Callback for setting up the sc_pool_elmnt_t. Returning non-zero skips tracing of this syscall.
	sc_callback *prepare_cb;
	// Callback for custom dumping (before normal dumping). Returning non-zero skips the default printer, and the post_writer_cb
	sc_callback *pre_writer_cb;
	// Callback for custom dumping (after normal dumping). Return value ignored.
	sc_callback *post_writer_cb;
	
	char* arg_fmt[SCI_ARGS_MAX]; // Information about the various parameters
} syscall_info_t;

extern syscall_info_t *syscall_info;


/* Methods */
extern int init_syscall_info(void);
extern syscall_info_t* get_syscall_info(uint16_t num);

#endif
