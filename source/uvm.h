#pragma once

/* **** */

typedef struct vm_tag* vm_ptr;
typedef vm_ptr const vm_ref;

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

typedef enum opcode_class_enum {
	_alu, _alu_s, _alu_ea,
	_get_rm, _get_rn, _get_rs,
	_ldst,
	_misc,
	_skip_cc,
	_sop,
	_wb_rd, _wb_sop,
//
	__OPCODE_CLASS_COUNT,
}opcode_class_t;

typedef enum opcode_condition_enum {
	_cc_eq, _cc_ne,
	_cc_cs, _cc_cc,
	_cc_mi, _cc_pl,
	_cc_vs, _cc_vc,
	_cc_hi, _cc_ls,
	_cc_ge, _cc_lt,
	_cc_gt, _cc_le,
	_cc_al, _cc_nv,
//
	_cc_eq_ne = _cc_eq,
	_cc_cs_cc = _cc_cs,
	_cc_mi_pl = _cc_mi,
	_cc_vs_vc = _cc_vs,
	_cc_hi_ls = _cc_ls,
	_cc_ge_lt = _cc_ge,
	_cc_gt_le = _cc_gt,
	_cc_al_nv = _cc_al,
}opcode_condition_t;

typedef enum opcode_alu_enum {
	_alu_and, _alu_eor,	_alu_sub, _alu_rsb,
	_alu_add, _alu_adc,	_alu_sbc, _alu_rsc,
	_alu_tst, _alu_teq,	_alu_cmp, _alu_cmn,
	_alu_orr, _alu_mov, _alu_bic, _alu_mvn,
//
	_alu_clz, _alu_div, _alu_mod, _alu_mul,
}opcode_alu_t;

typedef enum opcode_ldst_enum {
	_ldb, _ldsb, _stb,
	_ldh, _ldsh, _sth,
	_ldr, _ldsr, _str,
}opcode_ldst_t;

typedef enum opcode_misc_enum {
	_misc_break, _misc_exit,
	_misc_get_rm_i16, _misc_get_rm_i32, _misc_get_rm_i8,
	_misc_get_rm_u8,
	_misc_get_rs_i8,
}opcode_misc_t;

typedef enum opcode_sop_enum {
	_sop_lsl, _sop_lsr, _sop_asr, _sop_ror, _sop_rrx,
	_alu_lsl = 8, _alu_lsr, _alu_asr, _alu_ror, _alu_rrx,
}opcode_sop_t;

typedef union uvm_pc_tag {
	void *p;
	int16_t* p2i16;
	uint16_t* p2u16;
	int32_t* p2i32;
	uint32_t* p2u32;
	int8_t* p2i8;
	uint8_t* p2u8;
}uvm_pc_t;

typedef struct vm_tag {
	unsigned long gpr[16];
#define GPRx(_x) vm->gpr[_x & 15]
#define rGPR(_x) GPRx(r##_x)
//
	unsigned long spr[_spr_rcount_];
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
		unsigned cf:1;
		unsigned nf:1;
		unsigned vf:1;
		unsigned zf:1;
	}cpsr;

	struct {
		void* data;
		void* param;
	}mem;
	
	uint8_t ucode[256][16];
}vm_t;

#define CYCLE rSPR64(cycle)
#define ICOUNT rSPR64(icount)
#define IP rSPR(ip)
#define IR rSPR(ir)
#define LR rGPR(LR)
#define PC rGPR(PC)
#define SP rGPR(SP)
