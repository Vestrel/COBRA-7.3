#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/libc.h>
#include <lv2/memory.h>

#include "utilities.h"

/*
 * Dumping helper methods
 */

// NOTE: Caller must make sure dest is large enough to fit the resulting string
void dump_to_string(void* mem, char* dest, uint32_t size)
{
	uint8_t *mem8 = (uint8_t*)mem;
	
	for(uint32_t i = 0; i < size; i++)
	{
		dest += lv2_sprintf(dest, "%02x", *mem8);
		mem8 += 1;
	}
}

int memory_printf(void *mem, uint32_t size, char *prefix)
{
	uint32_t buf_size = size * 2 + 1; // two characters per byte, plus null terminator

	char* buf = (char*)alloc(buf_size, 0x27);

	if(!buf) {
		lv2_printf("memory_printf: alloc call failed\n");
		return 0;
	}

	dump_to_string(mem, buf, size);
	lv2_printf("%s%s\n", prefix, buf);
	
	dealloc(buf, 0x27);
	return 1;
}
