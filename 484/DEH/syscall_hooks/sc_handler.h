#ifndef __SC_HANDLER_H__
#define __SC_HANDLER_H__

/*
 * Defines
 */

// Skips the pre-send callback
//#define SC_HANDLER_NO_CALLBACKS

// Only trace this specific process
#define SC_TRACE_ONLY_PROCESS "vsh"


/*
 * Methods
 */
extern int init_syscall_handler(void);


#endif // __SC_HANDLER_H__
