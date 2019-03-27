#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/syscall.h>

#include "utilities.h"
#include "sc_writer.h"
#include "sc_pool.h"
#include "sc_handler.h"
#include "sc_info.h"
#include "sc_sys_hid.h"
#include "sc_sys_config.h"
#include "sc_sys_event_queue.h"


/*
 * Enables the hook
 */
void main(void)
{
	PRINT("Initializing...\n");

	if(init_syscall_info() != 0)
		return;

	if(sc_sys_hid_init() != 0)
		return;

	if(sc_sys_config_init() != 0)
		return;

	if(sc_sys_event_queue_init() != 0)
		return;

	if(init_syscall_pool() != 0)
		return;
	
	if(init_syscall_writer() != 0)
		return;

	if(init_syscall_handler() != 0)
		return;

	PRINT("Initialization complete.\n");
}
