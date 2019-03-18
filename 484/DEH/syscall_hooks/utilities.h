#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <lv2/libc.h>

/*
 * Macros
 */
#define STR(x) ((x) ? (char*)(x) : "NULL")

#define PRINTF(x, ...) lv2_printf("SCH: " x "\n", ## __VA_ARGS__) 


/*
 * Dumping helper methods
 */
void dump_to_string(void* mem, char* dest, uint32_t size);
int memory_printf(void *mem, uint32_t size, char *prefix);



#endif // __UTILITIES_H__
