#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/io.h>
#include <lv2/synchronization.h>
#include <lv2/memory.h>
#include <lv2/thread.h>

#include "utilities.h"
#include "syscall_writer.h"


/*
 * Defines
 */
#define SC_POOL_BUF_COUNT 128
#define SC_POOL_QUEUE_SIZE SC_POOL_BUF_COUNT+1
#define SC_WRITER_FILE "/app_home/syscall.log"


/*
 * Types
 */
typedef struct sc_buffer_pool_t {
	sc_buffer_t buffers[SC_POOL_BUF_COUNT];
} sc_buffer_pool_t;

typedef enum sc_buffer_type_e {
	SC_DEFAULT = 0,
	SC_FREE,
	SC_POOL
} sc_buffer_type_e;


/*
 * Variables
 */
static sc_buffer_pool_t *sc_buffer_pool;
static event_queue_t sc_evq;
static event_port_t sc_evp;

static thread_t sc_writer_thread;
static int sc_fd;


/*
 * Writer Thread
 */
static void sc_write(char *buf, uint64_t size) {
	if(!sc_fd)
		return;

	int err = cellFsWrite(sc_fd, buf, size, NULL);

	if(err) {
		ERROR("sc_log cellFsWrite failed (err=%x)! Stopping syscall logging.", err);
		cellFsClose(sc_fd);
		sc_fd = 0;
	}

	//lv2_printf(buf);
}


static void sc_writer_thread_main(uint64_t arg) {
	INFO("sc_writer_thread running!");

	for(;;) {
		event_t ev;
		sc_buffer_t *scb;
		char *buf;

		// Wait for event
		int err = event_queue_receive(sc_evq, &ev, 0);
		if(err != 0) {
			ERROR("event_queue_receive returned error %x", err);
			ppu_thread_exit(1);
			return;
		}
		//INFO("queue returned");

		sc_buffer_type_e type = (sc_buffer_type_e)(ev.data2);

		// Grab buffer information
		if(type == SC_POOL) {
			scb = (sc_buffer_t*)(ev.data1);
			buf = scb->p;
		}
		else {
			scb = NULL;
			buf = (char*)(ev.data1);
		}

		// Write buffer
		if(buf != NULL)
			sc_write(buf, strlen(buf));

		//INFO("Wrote buf = %s", buf);

		// Release buffer
		if(type == SC_POOL)
			scb->in_use = 0;
		else if(type == SC_FREE)
			free(buf);
	}

	ppu_thread_exit(0);
}


/*
 * Initialization
 */
int init_syscall_writer(void) {
	// Buffer pool
	sc_buffer_pool = (sc_buffer_pool_t*)malloc(sizeof(sc_buffer_pool_t));
	if(sc_buffer_pool == NULL) {
		ERROR("Could not allocate sc_buffer_pool.");
		return -1;
	}
	memset(sc_buffer_pool, 0, sizeof(sc_buffer_pool_t));

	// Event queue & port for inter-thread communication
	int err = event_queue_create(&sc_evq, SYNC_PRIORITY, /*event_queue_key*/ 1, /*size 1-127*/ SC_POOL_QUEUE_SIZE > 127 ? 127 : SC_POOL_QUEUE_SIZE );
	if(err != 0) {
		ERROR("Could not initialize sc_evq (err=%x)", err);
		return -2;
	}

	err = event_port_create(&sc_evp, EVENT_PORT_REMOTE);
	if(err != 0) {
		ERROR("Could not initialize sc_evp (err=%x)", err);
		return -3;
	}

	err = event_port_connect(sc_evp, sc_evq);
	if(err != 0) {
		ERROR("Could not connect sc_evp to sc_evq (err=%x)", err);
		return -4;
	}

	// Open log file
	sc_fd = 0;
	err = cellFsOpen(SC_WRITER_FILE, CELL_FS_O_WRONLY | CELL_FS_O_CREAT | CELL_FS_O_TRUNC, &sc_fd, 0666, NULL, 0);
	if(err) {
		ERROR("Could not open/create syscall log file '" SC_WRITER_FILE "' (err=%x).", err);
		return -5;
	}
	if(!sc_fd) {
		ERROR("Could not open/create syscall log file '" SC_WRITER_FILE "' (fd==0).");
		return -6;
	}

	// Create writer thread
	err = ppu_thread_create(&sc_writer_thread, &sc_writer_thread_main, /*arg*/ 0, /*prio*/ 1000, /*stacksize*/ 0x4000, 0, "SCH_sc_writer");
	if(err != 0) {
		ERROR("Could not create sc_writer_thread (err=%x)", err);
		return -7;
	}

	// Success
	INFO("SC Buffer Pool initialized!\nSyscalls will be logged to file '" SC_WRITER_FILE "'.");
	return 0;
}


/*
 * Utility Methods
 */
sc_buffer_t* sc_pool_get_buffer(void) {
	// Enter critical section
	uint64_t irqsave = spin_lock_irqsave();

	// Search for free buffer slot
	sc_buffer_t *scb = sc_buffer_pool->buffers;
	char *err = NULL;

	uint16_t i;
	for(i = 0; i < SC_POOL_BUF_COUNT; i++, scb++) {
		if(!scb->in_use)
			break;
	}

	// Reserve/allocate buffer
	if(i < SC_POOL_BUF_COUNT) {
		// Reserve buffer
		scb->in_use = 1;

		// Allocate buffer if necessary
		if(scb->p == NULL) {
			scb->p = (char*)malloc(SC_POOL_BUF_LEN * sizeof(char));

			if(scb->p == NULL) {
				scb->in_use = 0;
				scb = NULL;
				err = ">>> ERROR: Could not allocate SC buffer <<<\n";
			}
		}
	}
	else {
		scb = NULL;
		err = ">>> ERROR: Out of SC buffers <<<\n";
	}

	// Exit critical section
	spin_unlock_irqrestore(irqsave);

	// Return
	if(err != NULL) {
		sc_send_buffer(err);
		return NULL;
	}

	return scb;
}

void sc_send_buffer(const char *buf) {
	event_port_send(sc_evp, (uint64_t)buf, (uint64_t)SC_DEFAULT, 0);
}

void sc_send_and_free_buffer(char *buf) {
	event_port_send(sc_evp, (uint64_t)buf, (uint64_t)SC_FREE, 0);
}

void sc_pool_send_buffer(sc_buffer_t *buf) {
	event_port_send(sc_evp, (uint64_t)buf, (uint64_t)SC_POOL, 0);
}
