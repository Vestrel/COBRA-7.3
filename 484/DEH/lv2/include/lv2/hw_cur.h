#ifndef __HW_CUR_H__
#define __HW_CUR_H__

#include "lv2.h"
#include "thread.h"

typedef struct HW_cur_unk68_t {
	thread_obj_t *thread_obj;
} HW_cur_unk68_t;

typedef struct HW_cur_unkA0_t {
	uint8_t unk0[0x40];
	ppu_sched_obj_t *ppu_sched_obj;
} HW_cur_unkA0_t;

typedef struct HW_cur_t {
	uint64_t id; // hardware thread id
	uint8_t unk8[0x60];
	HW_cur_unk68_t *unk68;
	uint8_t unk76[0x40];
	HW_cur_unkA0_t *unkA0;
} HW_cur_t;



static INLINE HW_cur_t* get_HW_cur(void)
{
	return (HW_cur_t*)(mfsprg0());
}

static INLINE thread_obj_t *get_thread_obj(void)
{
	return get_HW_cur()->unk68->thread_obj;
}

static INLINE ppu_sched_obj_t *get_ppu_sched_obj(void)
{
	return get_HW_cur()->unkA0->ppu_sched_obj;
}


#endif // __HW_CUR_H__
