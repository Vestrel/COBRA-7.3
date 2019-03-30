#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc_info.h"
#include "utilities.h"

#include "sc_sys_net.h"

int sc_sys_net_init(void) {
	SCI_CREATE_GROUP(sys_net, SCT_TRACE_PRE);

	// s32 sys_net_bnet_accept(ppu_thread&, s32 s, vm::ptr<sys_net_sockaddr> addr, vm::ptr<u32> paddrlen);
	SCI_START(sys_net_bnet_accept)
		s->nargs = 3;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "addr=*0x%lx";
		s->arg_fmt[2] = "paddrlen=*0x%lx";
	SCI_END

	// s32 sys_net_bnet_bind(ppu_thread&, s32 s, vm::cptr<sys_net_sockaddr> addr, u32 addrlen);
	SCI_START(sys_net_bnet_bind)
		s->nargs = 3;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "addr=*0x%lx";
		s->arg_fmt[2] = "addrlen=0x%lx";
	SCI_END

	// s32 sys_net_bnet_connect(ppu_thread&, s32 s, vm::ptr<sys_net_sockaddr> addr, u32 addrlen);
	SCI_START(sys_net_bnet_connect)
		s->nargs = 3;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "addr=*0x%lx";
		s->arg_fmt[2] = "addrlen=0x%lx";
	SCI_END

	// s32 sys_net_bnet_getpeername(ppu_thread&, s32 s, vm::ptr<sys_net_sockaddr> addr, vm::ptr<u32> paddrlen);
	SCI_START(sys_net_bnet_getpeername)
		s->nargs = 3;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "addr=*0x%lx";
		s->arg_ptr[2] = 4;
		s->arg_fmt[2] = "paddrlen=*0x%lx->0x%x";
		s->trace = SCT_TRACE_POST;
	SCI_END

	// s32 sys_net_bnet_getsockname(ppu_thread&, s32 s, vm::ptr<sys_net_sockaddr> addr, vm::ptr<u32> paddrlen);
	SCI_START(sys_net_bnet_getsockname)
		s->nargs = 3;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "addr=*0x%lx";
		s->arg_fmt[2] = "paddrlen=*0x%lx";
	SCI_END

	// s32 sys_net_bnet_getsockopt(ppu_thread&, s32 s, s32 level, s32 optname, vm::ptr<void> optval, vm::ptr<u32> optlen);
	SCI_START(sys_net_bnet_getsockopt)
		s->nargs = 5;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "level=*0x%lx";
		s->arg_fmt[2] = "optname=*0x%lx";
		s->arg_fmt[3] = "optval=*0x%lx";
		s->arg_fmt[4] = "optlen=*0x%lx";
	SCI_END

	// s32 sys_net_bnet_listen(ppu_thread&, s32 s, s32 backlog);
	SCI_START(sys_net_bnet_listen)
		s->nargs = 2;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "backlog=0x%lx";
	SCI_END

	// s32 sys_net_bnet_recvfrom(ppu_thread&, s32 s, vm::ptr<void> buf, u32 len, s32 flags, vm::ptr<sys_net_sockaddr> addr, vm::ptr<u32> paddrlen);
	SCI_START(sys_net_bnet_recvfrom)
		s->nargs = 6;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "buf=*0x%lx";
		s->arg_fmt[2] = "len=0x%lx";
		s->arg_fmt[3] = "flags=0x%lx";
		s->arg_fmt[4] = "addr=*0x%lx";
		s->arg_fmt[5] = "paddrlen=*0x%lx";
	SCI_END

	// s32 sys_net_bnet_recvmsg(ppu_thread&, s32 s, vm::ptr<sys_net_msghdr> msg, s32 flags);
	SCI_START(sys_net_bnet_recvmsg)
		s->nargs = 3;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "msg=*0x%lx";
		s->arg_fmt[2] = "flags=0x%lx";
	SCI_END

	// s32 sys_net_bnet_sendmsg(ppu_thread&, s32 s, vm::cptr<sys_net_msghdr> msg, s32 flags);
	SCI_START(sys_net_bnet_sendmsg)
		s->nargs = 3;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "msg=*0x%lx";
		s->arg_fmt[2] = "flags=0x%lx";
	SCI_END

	// s32 sys_net_bnet_sendto(ppu_thread&, s32 s, vm::cptr<void> buf, u32 len, s32 flags, vm::cptr<sys_net_sockaddr> addr, u32 addrlen);
	SCI_START(sys_net_bnet_recvfrom)
		s->nargs = 6;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "buf=*0x%lx";
		s->arg_fmt[2] = "len=0x%lx";
		s->arg_fmt[3] = "flags=0x%lx";
		s->arg_fmt[4] = "addr=*0x%lx";
		s->arg_fmt[5] = "addrlen=0x%lx";
	SCI_END

	// s32 sys_net_bnet_setsockopt(ppu_thread&, s32 s, s32 level, s32 optname, vm::cptr<void> optval, u32 optlen);
	SCI_START(sys_net_bnet_getsockname)
		s->nargs = 5;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "level=*0x%lx";
		s->arg_fmt[2] = "optname=*0x%lx";
		s->arg_fmt[3] = "optval=*0x%lx";
		s->arg_fmt[4] = "optlen=*0x%lx";
	SCI_END

	// s32 sys_net_bnet_shutdown(ppu_thread&, s32 s, s32 how);
	SCI_START(sys_net_bnet_shutdown)
		s->nargs = 2;
		s->arg_fmt[0] = "s=0x%lx";
		s->arg_fmt[1] = "how=0x%lx";
	SCI_END

	// s32 sys_net_bnet_socket(ppu_thread&, s32 family, s32 type, s32 protocol);
	SCI_START(sys_net_bnet_socket)
		s->nargs = 3;
		s->arg_fmt[0] = "family=0x%lx";
		s->arg_fmt[1] = "type=0x%lx";
		s->arg_fmt[2] = "protocol=0x%lx";
	SCI_END

	// s32 sys_net_bnet_close(ppu_thread&, s32 s);
	SCI_START(sys_net_bnet_close)
		s->nargs = 1;
		s->arg_fmt[0] = "s=0x%lx";
	SCI_END

	// s32 sys_net_bnet_poll(ppu_thread&, vm::ptr<sys_net_pollfd> fds, s32 nfds, s32 ms);
	SCI_START(sys_net_bnet_poll)
		s->nargs = 3;
		s->arg_fmt[0] = "fds=*0x%lx";
		s->arg_fmt[1] = "nfds=0x%lx";
		s->arg_fmt[2] = "ms=0x%lx";
	SCI_END

	// s32 sys_net_bnet_select(ppu_thread&, s32 nfds, vm::ptr<sys_net_fd_set> readfds, vm::ptr<sys_net_fd_set> writefds, vm::ptr<sys_net_fd_set> exceptfds, vm::ptr<sys_net_timeval> timeout);
	SCI_START(sys_net_bnet_select)
		s->nargs = 3;
		s->arg_fmt[0] = "readfds=*0x%lx";
		s->arg_fmt[1] = "writefds=*0x%lx";
		s->arg_fmt[2] = "exceptfds=*0x%lx";
		s->arg_fmt[3] = "timeout=*0x%lx";
	SCI_END

	// s32 _sys_net_open_dump(ppu_thread&, s32 len, s32 flags);
	// s32 _sys_net_read_dump(ppu_thread&, s32 id, vm::ptr<void> buf, s32 len, vm::ptr<s32> pflags);
	// s32 _sys_net_close_dump(ppu_thread&, s32 id, vm::ptr<s32> pflags);
	// s32 _sys_net_write_dump(ppu_thread&, s32 id, vm::cptr<void> buf, s32 len, u32 unknown);

	// s32 sys_net_abort(ppu_thread&, s32 type, u64 arg, s32 flags);
	SCI_START(sys_net_abort)
		s->nargs = 3;
		s->arg_fmt[0] = "type=0x%lx";
		s->arg_fmt[1] = "arg=0x%lx";
		s->arg_fmt[2] = "flags=0x%lx";
	SCI_END

	// s32 sys_net_infoctl(ppu_thread&, s32 cmd, vm::ptr<void> arg);
	SCI_START(sys_net_infoctl)
		s->nargs = 2;
		s->arg_fmt[0] = "cmd=0x%lx";
		s->arg_fmt[1] = "arg=*0x%lx";
	SCI_END

	// s32 sys_net_control(ppu_thread&, u32 arg1, s32 arg2, vm::ptr<void> arg3, s32 arg4);
	SCI_START(sys_net_control)
		s->nargs = 4;
		s->arg_fmt[0] = "arg1=0x%lx";
		s->arg_fmt[1] = "arg2=0x%lx";
		s->arg_fmt[2] = "arg3=*0x%lx";
		s->arg_fmt[3] = "arg4=0x%lx";
	SCI_END

	// s32 sys_net_bnet_ioctl(ppu_thread&, s32 arg1, u32 arg2, u32 arg3);
	SCI_START(sys_net_bnet_ioctl)
		s->nargs = 3;
		s->arg_fmt[0] = "arg1=0x%lx";
		s->arg_fmt[1] = "arg2=0x%lx";
		s->arg_fmt[2] = "arg3=0x%lx";
	SCI_END

	// s32 sys_net_bnet_sysctl(ppu_thread&, u32 arg1, u32 arg2, u32 arg3, vm::ptr<void> arg4, u32 arg5, u32 arg6);
	SCI_START(sys_net_bnet_sysctl)
		s->nargs = 6;
		s->arg_fmt[0] = "arg1=0x%lx";
		s->arg_fmt[1] = "arg2=0x%lx";
		s->arg_fmt[2] = "arg3=0x%lx";
		s->arg_fmt[3] = "arg4=*0x%lx";
		s->arg_fmt[4] = "arg5=0x%lx";
		s->arg_fmt[5] = "arg6=0x%lx";
	SCI_END

	// s32 sys_net_eurus_post_command(ppu_thread&, s32 arg1, u32 arg2, u32 arg3);
	SCI_START(sys_net_eurus_post_command)
		s->nargs = 3;
		s->arg_fmt[0] = "arg1=0x%lx";
		s->arg_fmt[1] = "arg2=0x%lx";
		s->arg_fmt[2] = "arg3=0x%lx";
	SCI_END

	return 0;
}
