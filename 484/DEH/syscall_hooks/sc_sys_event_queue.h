#ifndef __SC_SYS_EVENT_QUEUE_H__
#define __SC_SYS_EVENT_QUEUE_H__

#define SYS_EVENT_MAX_TRACED_QUEUES 128

extern uint32_t sc_traced_equeue_ids[SYS_EVENT_MAX_TRACED_QUEUES];

extern void sc_sys_event_queue_trace_id(uint32_t new_id);
extern int sc_sys_event_queue_init(void);

#endif // __SC_SYS_EVENT_QUEUE_H__
