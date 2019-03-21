#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/syscall.h>

#include "utilities.h"
#include "syscall_writer.h"
#include "syscall_handler.h"
#include "syscall_info.h"


/*
 * Enables the hook
 */
void main(void)
{
	PRINT("Initializing...\n");

	init_syscall_info();
	
	if(init_syscall_writer() != 0)
		return;

	if(init_syscall_handler() != 0)
		return;

	PRINT("Initialization complete.\n");
}
