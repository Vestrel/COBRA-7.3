#ifndef __SYSCALL_INFO_H__
#define __SYSCALL_INFO_H__

#include <lv2/syscall.h>

struct syscall_info_t;
#include "sc_pool.h"

/*
 * Defines
 */
#define SCI_ARGS_MAX 8

/*
 * Macros
 */
#define _SCI_CREATE_GROUP(name) \
	syscall_group_t *name = sci_create_group( #name ); \
	sci_add_prefix_to_group(name, #name);

#define SCI_CREATE_GROUP(name, _trace) do { \
		_SCI_CREATE_GROUP(name) \
		name->trace = _trace; \
	} while(0)

#define SCI_PRECALL_PREPARE_CB(name) int sci_precall_prepare_cb__##name(struct sc_pool_elmnt_t *pe, char *proc_nm, char *thrd_nm)
#define SCI_POSTCALL_PREPARE_CB(name) int sci_postcall_prepare_cb__##name(struct sc_pool_elmnt_t *pe, char *proc_nm, char *thrd_nm)
#define SCI_PRE_WRITE_CB(name) int sci_pre_write_cb__##name(struct sc_pool_elmnt_t *pe)
#define SCI_POST_WRITE_CB(name) int sci_post_write_cb__##name(struct sc_pool_elmnt_t *pe)

#define SCI_PRECALL_PREPARE_CB_ALIAS(name, alias) sc_handler_callback *sci_precall_prepare_cb__##name = sci_precall_prepare_cb__##alias;
#define SCI_POSTCALL_PREPARE_CB_ALIAS(name, alias) sc_handler_callback *sci_postcall_prepare_cb__##name = sci_postcall_prepare_cb__##alias;
#define SCI_PRE_WRITE_CB_ALIAS(name, alias) sc_writer_callback *sci_pre_write_cb__##name = sci_pre_write_cb__##alias;
#define SCI_POST_WRITE_CB_ALIAS(name, alias) sc_writer_callback *sci_post_write_cb__##name = sci_post_write_cb__##alias;

#define SCI_NULL_PRECALL_PREPARE_CB(name) sc_handler_callback *sci_precall_prepare_cb__##name = NULL;
#define SCI_NULL_POSTCALL_PREPARE_CB(name) sc_handler_callback *sci_postcall_prepare_cb__##name = NULL;
#define SCI_NULL_PRE_WRITE_CB(name) sc_writer_callback *sci_pre_write_cb__##name = NULL;
#define SCI_NULL_POST_WRITE_CB(name) sc_writer_callback *sci_post_write_cb__##name = NULL;

#define SCI_FROM_NAME(name) \
	syscall_info_t *name = sci_get_sc_by_name(#name); \
	if(name == NULL) { \
		ERROR("Could not find syscall '" #name "'"); \
		return 0; \
	}

#define SCI_START(name) { \
	SCI_FROM_NAME(name) \
	syscall_info_t *s = name; \

#define SCI_END }

#define SCI_REGISTER_CBS(name) \
	name->precall_prepare_cb = sci_precall_prepare_cb__##name; \
	name->postcall_prepare_cb = sci_postcall_prepare_cb__##name; \
	name->pre_write_cb = sci_pre_write_cb__##name; \
	name->post_write_cb = sci_post_write_cb__##name;


#define SCI_CB_DUMP_BUFFER_PRECALL_PREPARE(name, buf_arg, len_arg) \
	SCI_PRECALL_PREPARE_CB(name) { \
		sc_pe_add(pe, (void*)(pe->args[buf_arg]), pe->args[len_arg]); \
		return 0; \
	}

#define SCI_CB_DUMP_BUFFER_POSTCALL_PREPARE(name, buf_arg, len_arg) \
	SCI_POSTCALL_PREPARE_CB(name) { \
		sc_pe_add(pe, (void*)(pe->args[buf_arg]), pe->args[len_arg]); \
		return 0; \
	}

#define SCI_CB_DUMP_BUFFER_WRITE(buf) \
	void *buf = NULL; \
	uint16_t buf##_len = 0; \
	if(sc_pe_next(pe, &buf, &buf##_len) != SC_PE_OK) { \
		ERROR(#buf ": Could not get " #buf " buffer"); \
		return 0; \
	} \
	\
	if(buf##_len > 0) { \
		SCW_START \
		SCW_PRINTF(#buf "= "); \
		SCW_DUMP(buf, buf##_len); \
		SCW_PRINTF("\n"); \
		SCW_FINISH \
	}

#define SCI_CB_PRECALL_DUMP_BUFFER(name, buf_arg, len_arg) \
	SCI_CB_DUMP_BUFFER_PRECALL_PREPARE(name, buf_arg, len_arg) \
	\
	SCI_NULL_POSTCALL_PREPARE_CB(name) \
	\
	SCI_NULL_PRE_WRITE_CB(name) \
	\
	SCI_POST_WRITE_CB(name) { \
		SCI_CB_DUMP_BUFFER_WRITE(pre) \
		return 0; \
	}

#define SCI_CB_POSTCALL_DUMP_BUFFER(name, buf_arg, len_arg) \
	SCI_NULL_PRECALL_PREPARE_CB(name) \
	\
	SCI_CB_DUMP_BUFFER_POSTCALL_PREPARE(name, buf_arg, len_arg) \
	\
	SCI_NULL_PRE_WRITE_CB(name) \
	\
	SCI_POST_WRITE_CB(name) { \
		SCI_CB_DUMP_BUFFER_WRITE(post) \
		return 0; \
	}

#define SCI_CB_DUMP_BUFFER(name, buf_arg, len_arg) \
	SCI_CB_DUMP_BUFFER_PRECALL_PREPARE(name, buf_arg, len_arg) \
	\
	SCI_CB_DUMP_BUFFER_POSTCALL_PREPARE(name, buf_arg, len_arg) \
	\
	SCI_NULL_PRE_WRITE_CB(name) \
	\
	SCI_POST_WRITE_CB(name) { \
		SCI_CB_DUMP_BUFFER_WRITE(pre) \
		SCI_CB_DUMP_BUFFER_WRITE(post) \
		return 0; \
	}

/*
 * Types
 */
typedef enum syscall_trace_e {
	SCT_DONT_TRACE = 0,
	SCT_DEFAULT,
	SCT_TRACE_PRE,
	SCT_TRACE_POST
} syscall_trace_e;

typedef uint8_t syscall_group_id_t;

typedef struct syscall_group_t {
	syscall_group_id_t id;
	syscall_trace_e trace;

	char *name;
} syscall_group_t;

// Callback type for custom dumping
// If it returns 0, the default dumper is still executed
typedef int (sc_handler_callback)(struct sc_pool_elmnt_t *pe, char *proc_nm, char *thrd_nm);
typedef int (sc_writer_callback)(struct sc_pool_elmnt_t *pe);

typedef struct syscall_info_t {
	uint16_t num;
	uint8_t nargs;

	syscall_trace_e trace; // Whether to trace this syscall

	char *name;

	syscall_group_t *group;

	// Callback for setting up the sc_pool_elmnt_t. Returning non-zero skips tracing of this syscall.
	sc_handler_callback *precall_prepare_cb;
	// Callback for setting up the sc_pool_elmnt_t. Returning non-zero skips tracing of this syscall. Only called if trace==SCT_TRACE_POST
	sc_handler_callback *postcall_prepare_cb;

	// Callback for custom dumping (before normal dumping). Returning non-zero skips the default printer, and the post_writer_cb
	sc_writer_callback *pre_write_cb;
	// Callback for custom dumping (after normal dumping). Return value ignored.
	sc_writer_callback *post_write_cb;
	
	char* arg_fmt[SCI_ARGS_MAX]; // Information about the various parameters
	uint8_t arg_ptr[SCI_ARGS_MAX]; // If 1, the value pointed to by the arg is also passed to printf. Requires trace=SCT_TRACE_POST
} syscall_info_t;

extern syscall_info_t *syscall_info;


/*
 * Methods
 */
extern int init_syscall_info(void);
extern syscall_info_t* sci_get_sc(uint16_t sc);
extern syscall_info_t* sci_get_sc_by_name(char *nm);
extern syscall_trace_e sci_get_trace(syscall_info_t *info);

extern syscall_group_t* sci_create_group(char *nm);
extern syscall_group_t* sci_find_group(char *nm);

extern void sci_add_to_group(syscall_group_t* group, uint16_t sc);
extern void sci_add_range_to_group(syscall_group_t* group, uint16_t sc_start, uint16_t sc_end);
extern void sci_trace_group(syscall_group_t* group, syscall_trace_e trace);

extern void sci_trace(uint16_t sc, syscall_trace_e trace);
extern void sci_trace_range(uint16_t sc_start, uint16_t sc_end, syscall_trace_e trace);

extern void sci_add_prefix_to_group(syscall_group_t *group, char *prefix);

#endif
