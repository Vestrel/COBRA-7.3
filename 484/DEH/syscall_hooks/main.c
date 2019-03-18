#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/syscall.h>

#include "utilities.h"
#include "syscall_handler.h"
#include "syscall_info.h"


/*
 * Enables the hook
 */
void main(void)
{
	PRINTF("Initializing...\n");
	
	init_syscall_info();
	init_syscall_handler();

	PRINTF("Initialization complete.\n");
}
