#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/time.h>

#include "sc_info.h"
#include "utilities.h"
#include "sc_sys_uart.h"

typedef struct ps3av_pkt_reply_hdr
{
	uint16_t version;
	uint16_t length;
	uint32_t cid;
	uint32_t status;
} ps3av_pkt_reply_hdr;

typedef struct ps3av_header
{
	uint16_t version;
	uint16_t length;
	uint32_t cid;
} ps3av_header;

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

        uint16_t crnt_offset = 0;

        while (buf_len >= sizeof(ps3av_pkt_reply_hdr))
        {
            const ps3av_pkt_reply_hdr *pkt = (ps3av_pkt_reply_hdr *)((size_t)(buf) + crnt_offset);
            void *pkt_data = (void *)((size_t)(pkt) + sizeof(ps3av_pkt_reply_hdr));
            const uint16_t pkt_total_size = pkt->length + 4;

            if (buf_len >= pkt_total_size)
            {
                SCW_PRINTF("pkt FROM lv1 at 0x%lx\n\t\tcid=0x%08x status=0x%08x size=%hu\n", get_ticks(), pkt->cid, pkt->status, pkt->length);

                if (pkt->length > 8)
                {
                    SCW_PRINTF("\t\tbuf=");
                    SCW_DUMP(pkt_data, pkt->length - 8);
                    SCW_PRINTF("\n");
                }

                buf_len -= pkt_total_size;
                crnt_offset += pkt_total_size;
            }
            else
            {
                break;
            }
        }

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

        uint16_t crnt_offset = 0;

        while (buf_len >= sizeof(ps3av_header))
        {
            const ps3av_header *pkt = (ps3av_header *)((size_t)(buf) + crnt_offset);
            void *pkt_data = (void *)((size_t)(pkt) + sizeof(ps3av_header));
            const uint16_t pkt_total_size = pkt->length + 4;

            if (buf_len >= pkt_total_size)
            {
                SCW_PRINTF("pkt TO lv1 at 0x%lx\n\t\tcid=0x%08x size=%hu\n", get_ticks(), pkt->cid, pkt->length);

                if (pkt->length > 4)
                {
                    SCW_PRINTF("\t\tbuf=");
                    SCW_DUMP(pkt_data, pkt->length - 4);
                    SCW_PRINTF("\n");
                }

                buf_len -= pkt_total_size;
                crnt_offset += pkt_total_size;
            }
            else
            {
                break;
            }
        }

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
