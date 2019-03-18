#ifndef __SYSCALL_HANDLER_H__
#define __SYSCALL_HANDLER_H__

#include <lv2/syscall.h>
#include <lv2/thread.h>

struct syscall_t;
#include "syscall_info.h"

/* Types */


// Structure describing the current syscall being processed
typedef struct syscall_t {
	char           *proc;
	thread_s       *thr;
	struct syscall_info_t *info;

	uint64_t  num;
	uint64_t  res;
	uint64_t  args[8];
} syscall_t;


/* Methods */
extern void init_syscall_handler(void);
extern void default_syscall_printer(syscall_t *sc);
extern void legacy_syscall_printer(syscall_t *sc);


#include "syscall_info.h"

#endif // __SYSCALL_HANDLER_H__
