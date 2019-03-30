#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_info.h"
#include "utilities.h"
#include "sc_sys_event_queue.h"

#include "sc_sys_config.h"


// sys_config_open
SCI_NULL_PRECALL_PREPARE_CB(sys_config_open)
SCI_POSTCALL_PREPARE_CB(sys_config_open) {
	if(pe->res == 0)
		sc_sys_event_queue_trace_id((uint32_t)(pe->args[0]));
	return 0;
}
SCI_NULL_PRE_WRITE_CB(sys_config_open)
SCI_NULL_POST_WRITE_CB(sys_config_open)

// sys_config_get_service_event
//SCI_CB_DUMP_BUFFER(sys_config_get_service_event, 2, 3)
typedef struct sys_config_service_event_t {
	uint32_t service_listener_handle;
	uint32_t event_added;
	uint64_t service_id;
	uint64_t user_id;
	/* if event_added==0, the following fields will not be present */
	uint64_t verbosity;
	uint32_t buf_size;
	uint32_t padding;
	uint8_t buf[256];
} sys_config_service_event_t;

SCI_NULL_PRECALL_PREPARE_CB(sys_config_get_service_event)
SCI_CB_DUMP_BUFFER_POSTCALL_PREPARE(sys_config_get_service_event, 2, 3)
SCI_NULL_PRE_WRITE_CB(sys_config_get_service_event)

SCI_POST_WRITE_CB(sys_config_get_service_event) {
	void *buf = NULL;
	uint16_t buf_len = 0;
	if(sc_pe_next(pe, &buf, &buf_len) != SC_PE_OK) {
		ERROR("sys_config_get_service_event: Could not get buffer");
		return 0;
	}

	if(buf_len > 0) {
		sys_config_service_event_t *sev = (sys_config_service_event_t*)buf;

		SCW_START

		SCW_PRINTF("listener= 0x%x    added= %d\n", sev->service_listener_handle, sev->event_added);
		SCW_PRINTF("\tsid= 0x%lx   user_id= 0x%lx\n", sev->service_id, sev->user_id);

		if(sev->event_added) {
			SCW_PRINTF("\tverbosity= 0x%lx   buf_size= 0x%x   padding= 0x%x\n", sev->verbosity, sev->buf_size, sev->padding);
			SCW_PRINTF("\tbuf= ");
			SCW_DUMP(sev->buf, sev->buf_size);
			SCW_PRINTF("\n");
		}

		SCW_FINISH
	}

	return 0;
}

// misc
SCI_CB_DUMP_BUFFER(sys_config_register_service, 4, 5)
SCI_CB_DUMP_BUFFER(sys_config_add_service_listener, 3, 4)


int sc_sys_config_init(void) {
	SCI_CREATE_GROUP(sys_config, SCT_TRACE_POST);

	SCI_FROM_NAME(sys_config_open)
	SCI_REGISTER_CBS(sys_config_open)
	sys_config_open->nargs = 2;
	sys_config_open->arg_fmt[0] = "equeue_id=0x%lx";
	sys_config_open->arg_ptr[1] = 4;
	sys_config_open->arg_fmt[1] = "config_id=*0x%lx->0x%x";
	sys_config_open->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_config_close)
	sys_config_close->nargs = 1;
	sys_config_close->arg_fmt[0] = "equeue_id=0x%lx";
	sys_config_close->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_config_register_service)
	SCI_REGISTER_CBS(sys_config_register_service)
	sys_config_register_service->nargs = 7;
	sys_config_register_service->arg_fmt[0] = "config_id=0x%lx";
	sys_config_register_service->arg_fmt[1] = "b=0x%lx";
	sys_config_register_service->arg_fmt[2] = "c=0x%lx";
	sys_config_register_service->arg_fmt[3] = "d=0x%lx";
	sys_config_register_service->arg_fmt[4] = "data=*0x%lx";
	sys_config_register_service->arg_fmt[5] = "size=%ld";
	sys_config_register_service->arg_ptr[6] = 4;
	sys_config_register_service->arg_fmt[6] = "output=*0x%lx->0x%x";
	sys_config_register_service->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_config_unregister_service)
	sys_config_unregister_service->nargs = 2;
	sys_config_unregister_service->arg_fmt[0] = "config_id=0x%lx";
	sys_config_unregister_service->arg_fmt[1] = "service_handle=0x%lx";
	sys_config_unregister_service->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_config_add_service_listener)
	SCI_REGISTER_CBS(sys_config_add_service_listener)
	sys_config_add_service_listener->nargs = 7;
	sys_config_add_service_listener->arg_fmt[0] = "config_id=0x%lx";
	sys_config_add_service_listener->arg_fmt[1] = "id=0x%lx";
	sys_config_add_service_listener->arg_fmt[2] = "c=0x%lx";
	sys_config_add_service_listener->arg_fmt[3] = "buf=0x%lx";
	sys_config_add_service_listener->arg_fmt[4] = "size=%ld";
	sys_config_add_service_listener->arg_fmt[5] = "f=0x%lx";
	sys_config_add_service_listener->arg_ptr[6] = 4;
	sys_config_add_service_listener->arg_fmt[6] = "handle=*0x%lx->0x%x";
	sys_config_add_service_listener->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_config_remove_service_listener)
	sys_config_remove_service_listener->nargs = 2;
	sys_config_remove_service_listener->arg_fmt[0] = "config_id=0x%lx";
	sys_config_remove_service_listener->arg_fmt[1] = "listener_handle=0x%lx";
	sys_config_remove_service_listener->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_config_get_service_event)
	SCI_REGISTER_CBS(sys_config_get_service_event)
	sys_config_get_service_event->nargs = 4;
	sys_config_get_service_event->arg_fmt[0] = "config_id=0x%lx";
	sys_config_get_service_event->arg_fmt[1] = "event_id=0x%lx";
	sys_config_get_service_event->arg_fmt[2] = "event=*0x%lx";
	sys_config_get_service_event->arg_fmt[3] = "size=%ld";
	sys_config_get_service_event->trace = SCT_TRACE_POST;

	return 0;
}
