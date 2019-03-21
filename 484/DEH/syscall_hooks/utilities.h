#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <lv2/libc.h>

/*
 * Macros
 */
#define STR(x) ((x) ? (char*)(x) : "NULL")

#define INFO(x, ...) lv2_printf("SCH: " x "\n", ## __VA_ARGS__)
#define PRINT(x, ...) INFO(x, ## __VA_ARGS__)
#define PRINTF(x, ...) INFO(x, ## __VA_ARGS__)

#define WARN(x, ...) lv2_printf("SCH *WARN*: " x "\n", ## __VA_ARGS__)
#define ERROR(x, ...) lv2_printf("SCH ***ERROR***: " x "\n", ## __VA_ARGS__)
#define FATAL(x, ...) do { lv2_printf("SCH ***FATAL***: " x "\n", ## __VA_ARGS__); lv1_panic(0); } while(0)


/*
 * Dumping helper methods
 */
void dump_to_string(void* mem, char* dest, uint32_t size);
int memory_printf(void *mem, uint32_t size, char *prefix);



#endif // __UTILITIES_H__
