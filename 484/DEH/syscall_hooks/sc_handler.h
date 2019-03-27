#ifndef __SC_HANDLER_H__
#define __SC_HANDLER_H__

/*
 * Defines
 */

// Skips the pre-send callback
//#define SC_HANDLER_NO_CALLBACKS

// Try to avoid growing the stack too much
// Disables reading return values passed by register
//#define SC_AVOID_LARGE_STACK


/*
 * Methods
 */
extern int init_syscall_handler(void);


#endif // __SC_HANDLER_H__
