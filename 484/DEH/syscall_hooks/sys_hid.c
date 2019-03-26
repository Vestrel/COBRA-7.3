#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_info.h"
#include "utilities.h"

#include "sys_hid.h"

/*
	error_code sys_hid_manager_open(u64 device_type, u64 port_no, vm::ptr<u32> handle);
	error_code sys_hid_manager_ioctl(u32 hid_handle, u32 pkg_id, vm::ptr<void> buf, u64 buf_size);
	error_code sys_hid_manager_add_hot_key_observer(u32 event_queue, vm::ptr<u32> unk);
	error_code sys_hid_manager_check_focus();
	error_code sys_hid_manager_is_process_permission_root();
	error_code sys_hid_manager_513(u64 a1, u64 a2, vm::ptr<void> buf, u64 buf_size);
	error_code sys_hid_manager_514(u32 pkg_id, vm::ptr<void> buf, u64 buf_size);
	error_code sys_hid_manager_read(u32 handle, u32 pkg_id, vm::ptr<void> buf, u64 buf_size);
*/

SCI_CB_DUMP_BUFFER(sys_hid_manager_ioctl, 2, 3)
SCI_CB_DUMP_BUFFER(sys_hid_manager_513, 2, 3)
SCI_CB_DUMP_BUFFER(sys_hid_manager_514, 1, 2)
SCI_CB_POSTCALL_DUMP_BUFFER(sys_hid_manager_read, 2, 3)


int sc_sys_hid_init(void) {
	SCI_CREATE_GROUP(sys_hid, SCT_DEFAULT);

	SCI_FROM_NAME(sys_hid_manager_open)
	sys_hid_manager_open->nargs = 3;
	sys_hid_manager_open->arg_fmt[0] = "device_type=0x%ld";
	sys_hid_manager_open->arg_fmt[1] = "port_no=%ld";
	sys_hid_manager_open->arg_ptr[2] = 4;
	sys_hid_manager_open->arg_fmt[2] = "handle=*0x%lx->0x%x";
	sys_hid_manager_open->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_hid_manager_add_hot_key_observer)
	sys_hid_manager_add_hot_key_observer->nargs = 2;
	sys_hid_manager_add_hot_key_observer->arg_fmt[0] = "event_queue=0x%lx";
	sys_hid_manager_add_hot_key_observer->arg_ptr[1] = 4;
	sys_hid_manager_add_hot_key_observer->arg_fmt[1] = "unk=*0x%lx->%x";
	sys_hid_manager_add_hot_key_observer->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_hid_manager_check_focus)
	sys_hid_manager_check_focus->nargs = 0;
	sys_hid_manager_check_focus->trace = SCT_DONT_TRACE;
	
	SCI_FROM_NAME(sys_hid_manager_grab_focus)
	sys_hid_manager_grab_focus->nargs = 1;
	sys_hid_manager_grab_focus->arg_fmt[0] = "0x%lx";
	sys_hid_manager_grab_focus->trace = SCT_DONT_TRACE;

	SCI_FROM_NAME(sys_hid_manager_is_process_permission_root)
	sys_hid_manager_is_process_permission_root->nargs = 0;

	SCI_FROM_NAME(sys_hid_manager_ioctl)
	SCI_REGISTER_CBS(sys_hid_manager_ioctl)
	sys_hid_manager_ioctl->nargs = 4;
	sys_hid_manager_ioctl->arg_fmt[0] = "hid_handle=0x%lx";
	sys_hid_manager_ioctl->arg_fmt[1] = "pkg_id=0x%lx";
	sys_hid_manager_ioctl->arg_fmt[2] = "buf=*0x%lx";
	sys_hid_manager_ioctl->arg_fmt[3] = "buf_size=%ld";
	sys_hid_manager_ioctl->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_hid_manager_513)
	SCI_REGISTER_CBS(sys_hid_manager_513)
	sys_hid_manager_513->nargs = 4;
	sys_hid_manager_513->arg_fmt[0] = "a1=0x%lx";
	sys_hid_manager_513->arg_fmt[1] = "a2=0x%lx";
	sys_hid_manager_513->arg_fmt[2] = "buf=*0x%lx";
	sys_hid_manager_513->arg_fmt[3] = "buf_size=%ld";
	sys_hid_manager_513->trace = SCT_TRACE_POST;

	SCI_FROM_NAME(sys_hid_manager_514)
	SCI_REGISTER_CBS(sys_hid_manager_514)
	sys_hid_manager_514->nargs = 3;
	sys_hid_manager_514->arg_fmt[0] = "pkg_id=0x%lx";
	sys_hid_manager_514->arg_fmt[1] = "buf=*0x%lx";
	sys_hid_manager_514->arg_fmt[2] = "buf_size=%ld";
	sys_hid_manager_514->trace = SCT_DONT_TRACE;

	SCI_FROM_NAME(sys_hid_manager_read)
	SCI_REGISTER_CBS(sys_hid_manager_read)
	sys_hid_manager_read->nargs = 4;
	sys_hid_manager_read->arg_fmt[0] = "handle=0x%lx";
	sys_hid_manager_read->arg_fmt[1] = "pkg_id=0x%lx";
	sys_hid_manager_read->arg_fmt[2] = "buf=*0x%lx";
	sys_hid_manager_read->arg_fmt[3] = "buf_size=%ld";
	sys_hid_manager_read->trace = SCT_TRACE_POST;

	return 0;
}
