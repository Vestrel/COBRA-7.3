#ifndef __SYSCALL_INFO_H__
#define __SYSCALL_INFO_H__

#include <lv2/syscall.h>

struct syscall_info_t;
#include "syscall_handler.h"

/*
 * Syscall Info structure
 */
#define SCI_ARGS_MAX 8

/* Types */

// Callback type for custom dumping
// If it returns 0, the default dumper is still executed
typedef int (sci_callback)(struct syscall_t *sc);

typedef struct syscall_info_t {
	uint16_t num;
	uint8_t nargs;

	uint8_t trace; // Whether to trace this syscall
	
	char *name;

	sci_callback *cb; // Callback for custom dumping
	
	char* arg_fmt[SCI_ARGS_MAX]; // Information about the various parameters
} syscall_info_t;

extern syscall_info_t *syscall_info;


/* Methods */
extern void init_syscall_info(void);
extern syscall_info_t* get_syscall_info(uint16_t num);

#include "syscall_handler.h"

#endif
