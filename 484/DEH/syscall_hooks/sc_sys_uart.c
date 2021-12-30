#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/time.h>

#include "sc_info.h"
#include "utilities.h"
#include "sc_sys_uart.h"

SCI_NULL_PRECALL_PREPARE_CB(sys_uart_receive)
SCI_CB_DUMP_BUFFER_POSTCALL_PREPARE(sys_uart_receive, 0, 1)
SCI_NULL_PRE_WRITE_CB(sys_uart_receive)
SCI_POST_WRITE_CB(sys_uart_receive)
{
    void *buf = NULL;
	uint16_t buf_len = 0;
	if (sc_pe_next(pe, &buf, &buf_len) != SC_PE_OK)
    {
		ERROR("sys_uart_receive: Could not get buffer");
		return 0;
	}

    buf_len = pe->res;

    if (buf_len > 0)
    {
        SCW_START

        SCW_PRINTF("pkt FROM lv1 at 0x%lx\n", get_ticks());
        SCW_PRINTF("\tbuf= ");
		SCW_DUMP(buf, buf_len);
        SCW_PRINTF("\n");

        SCW_FINISH
	}

    return 0;
}

SCI_NULL_PRECALL_PREPARE_CB(sys_uart_send)
SCI_CB_DUMP_BUFFER_POSTCALL_PREPARE(sys_uart_send, 0, 1)
SCI_NULL_PRE_WRITE_CB(sys_uart_send)
SCI_POST_WRITE_CB(sys_uart_send)
{
    void *buf = NULL;
	uint16_t buf_len = 0;
	if (sc_pe_next(pe, &buf, &buf_len) != SC_PE_OK)
    {
		ERROR("sys_uart_send: Could not get buffer");
		return 0;
	}

    buf_len = pe->res;

    if (buf_len > 0)
    {
        SCW_START

        SCW_PRINTF("pkt TO lv1 at 0x%lx\n", get_ticks());
        SCW_PRINTF("\tbuf= ");
		SCW_DUMP(buf, buf_len);
        SCW_PRINTF("\n");

        SCW_FINISH
	}

    return 0;
}

int sc_sys_uart_init(void)
{
    SCI_CREATE_GROUP(sys_config, SCT_TRACE_POST);

    SCI_FROM_NAME(sys_uart_receive)
    SCI_REGISTER_CBS(sys_uart_receive)
    sys_uart_receive->nargs = 3;
    sys_uart_receive->arg_fmt[0] = "buf_ptr=0x%lx";
    sys_uart_receive->arg_fmt[1] = "size=0x%lx";
    sys_uart_receive->arg_fmt[2] = "mode=0x%x";
    sys_uart_receive->trace = SCT_TRACE_POST;

    SCI_FROM_NAME(sys_uart_send)
    SCI_REGISTER_CBS(sys_uart_send)
    sys_uart_send->nargs = 3;
    sys_uart_send->arg_fmt[0] = "buf_ptr=0x%lx";
    sys_uart_send->arg_fmt[1] = "size=0x%lx";
    sys_uart_send->arg_fmt[2] = "mode=0x%x";
    sys_uart_send->trace = SCT_TRACE_POST;

    return 0;
}
