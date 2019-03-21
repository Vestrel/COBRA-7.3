#ifndef _SYSCALL_WRITER_H_
#define _SYSCALL_WRITER_H_


/*
 * Defines
 */
#define SC_POOL_BUF_LEN KB(4)


/*
 * Structures
 */

typedef struct {
	uint8_t in_use;
	char *p;
} sc_buffer_t;


/*
 * Methods
 */
extern int init_syscall_writer(void);
extern sc_buffer_t* sc_pool_get_buffer(void);
extern void sc_send_buffer(const char *buf);
extern void sc_send_and_free_buffer(char *buf);
extern void sc_pool_send_buffer(sc_buffer_t *buf);



#endif // _SYSCALL_WRITER_H_
