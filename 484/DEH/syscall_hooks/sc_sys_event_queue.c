#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_info.h"
#include "utilities.h"

#include "sc_sys_event_queue.h"

/*
	error_code sys_event_queue_create(vm::ptr<u32> equeue_id, vm::ptr<sys_event_queue_attribute_t> attr, u64 event_queue_key, s32 size);
	error_code sys_event_queue_destroy(ppu_thread& ppu, u32 equeue_id, s32 mode);
	error_code sys_event_queue_receive(ppu_thread& ppu, u32 equeue_id, vm::ptr<sys_event_t> dummy_event, u64 timeout);
	error_code sys_event_queue_tryreceive(u32 equeue_id, vm::ptr<sys_event_t> event_array, s32 size, vm::ptr<u32> number);
	error_code sys_event_queue_drain(u32 event_queue_id);
*/

uint32_t sc_traced_equeue_ids[SYS_EVENT_MAX_TRACED_QUEUES];

SCI_PRECALL_PREPARE_CB(sys_event_queue) {
	uint32_t equeue_id = (uint32_t)pe->args[0];

	for(uint16_t i = 0; i < SYS_EVENT_MAX_TRACED_QUEUES; i++) {
		if(sc_traced_equeue_ids[i] == equeue_id)
			return 0;
	}
	return 1;
}


// sys_event_queue_receive
SCI_PRECALL_PREPARE_CB_ALIAS(sys_event_queue_receive, sys_event_queue)
SCI_NULL_POSTCALL_PREPARE_CB(sys_event_queue_receive)
SCI_NULL_PRE_WRITE_CB(sys_event_queue_receive)
SCI_POST_WRITE_CB(sys_event_queue_receive) {
	SCW_START
	SCW_PRINTF("returned r4=%lx r5=%lx r6=%lx r7=%lx\n", pe->res_regs[0], pe->res_regs[1], pe->res_regs[2], pe->res_regs[3]);
	SCW_FINISH
	return 0;
}


// sys_event_queue_tryreceive
SCI_PRECALL_PREPARE_CB_ALIAS(sys_event_queue_tryreceive, sys_event_queue)
SCI_POSTCALL_PREPARE_CB(sys_event_queue_tryreceive) {
	sc_pe_add(pe, (void*)(pe->args[1]), pe->args[2]);
	sc_pe_add_u32(pe, *(uint32_t*)(pe->args[3]));
	return 0;
}

SCI_NULL_PRE_WRITE_CB(sys_event_queue_tryreceive)
SCI_POST_WRITE_CB(sys_event_queue_tryreceive) {
	void *buf = NULL;
	uint16_t buf_len = 0;
	if(sc_pe_next(pe, &buf, &buf_len) != SC_PE_OK) {
		ERROR("sys_event_queue_tryreceive: Could not get buffer");
		return 0;
	}

	uint32_t number = 0;
	sc_pe_next_u32(pe, &number);

	if(buf_len > 0) {
		SCW_START
		SCW_PRINTF("number=%d buf= ", number);
		SCW_DUMP(buf, buf_len);
		SCW_PRINTF("\n");
		SCW_FINISH
	}

	return 0;
}


// Utilities
void sc_sys_event_queue_trace_id(uint32_t new_id) {
	for(uint16_t i = 0; i < SYS_EVENT_MAX_TRACED_QUEUES; i++) {
		if(sc_traced_equeue_ids[i] == 0 || sc_traced_equeue_ids[i] == new_id) {
			sc_traced_equeue_ids[i] = new_id;
			break;
		}
	}
}

int sc_sys_event_queue_init(void) {
	SCI_CREATE_GROUP(sys_event_queue, SCT_DONT_TRACE);

	for(uint16_t i = 0; i < SYS_EVENT_MAX_TRACED_QUEUES; i++) {
		sc_traced_equeue_ids[i] = 0;
	}

	SCI_FROM_NAME(sys_event_queue_receive)
	SCI_REGISTER_CBS(sys_event_queue_receive)
	sys_event_queue_receive->nargs = 3;
	sys_event_queue_receive->arg_fmt[0] = "equeue_id=0x%lx";
	sys_event_queue_receive->arg_fmt[1] = "dummy_event=*0x%lx";
	sys_event_queue_receive->arg_fmt[2] = "timeout=0x%lx";
	sys_event_queue_receive->trace = SCT_TRACE_POST;


	SCI_FROM_NAME(sys_event_queue_tryreceive)
	SCI_REGISTER_CBS(sys_event_queue_tryreceive)
	sys_event_queue_tryreceive->nargs = 4;
	sys_event_queue_tryreceive->arg_fmt[0] = "equeue_id=0x%lx";
	sys_event_queue_tryreceive->arg_fmt[1] = "event_array=*0x%lx";
	sys_event_queue_tryreceive->arg_fmt[2] = "size=%ld";
	sys_event_queue_tryreceive->arg_fmt[3] = "number=*0x%lx";
	sys_event_queue_tryreceive->trace = SCT_TRACE_POST;

	return 0;
}
