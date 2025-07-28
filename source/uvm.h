#pragma once

/* **** */

#include "config.h"

typedef uvm_cell_st* uvm_cell_sptr;
typedef uvm_cell_sptr const uvm_cell_spref;
typedef uvm_cell_st const uvm_cell_stref;

typedef uvm_cell_ut* uvm_cell_uptr;
typedef uvm_cell_uptr const uvm_cell_upref;
typedef uvm_cell_ut const uvm_cell_utref;

typedef struct uvm_tag* uvm_ptr;
typedef uvm_ptr const uvm_ref;



#include <stdint.h>

/* **** */

enum {
	r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
//
	rLR = r13,
	rSP = r14,
	rPC = r15,
};

enum {
	_spr_ip,
	_spr_ir,
//
	_spr_rcount_,
};

enum {
	_spr32_rcount_,
};

enum {
	_spr64_cycle,
	_spr64_icount,
//
	_spr64_rcount_,
};

typedef union uvm_pc_tag {
	void *p;
	int16_t* p2i16;
	uint16_t* p2u16;
	int32_t* p2i32;
	uint32_t* p2u32;
	int8_t* p2i8;
	uint8_t* p2u8;
}uvm_pc_t;

typedef struct uvm_tag {
	uvm_cell_ut gpr[16];
#define GPRx(_x) vm->gpr[_x & 15]
#define rGPR(_x) GPRx(r##_x)
//
	uvm_cell_ut spr[_spr_rcount_];
#define SPRx(_x) vm->spr[_x]
#define rSPR(_x) SPRx(_spr_##_x)
//
	uint32_t spr32[_spr32_rcount_];
#define SPR32x(_x) vm->spr32[_x]
#define rSPR32(_x) SPR32x(_spr32_##_x)
//
	uint64_t spr64[_spr64_rcount_];
#define SPR64x(_x) vm->spr64[_x]
#define rSPR64(_x) SPR64x(_spr64_##_x)
//
	struct {
		struct {
			char c:1;
			char n:1;
			char v:1;
			char z:1;
		}apsr;
//
		char abort:1;
		char fiq:1;
		char irq:1;
		char wfi:1;
	}cpsr;

	struct {
		void* data;
		void* param;
	}mem;
	
	uint8_t ucode[256][16];
}uvm_t;

#define APSR(_flag) vm->cpsr.apsr._flag
#define CYCLE rSPR64(cycle)
#define ICOUNT rSPR64(icount)
#define IP rSPR(ip)
#define IR rSPR(ir)
#define LR rGPR(LR)
#define PC rGPR(PC)
#define SP rGPR(SP)
