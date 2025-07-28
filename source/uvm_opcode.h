#pragma once

/* **** */

typedef enum opcode_alu0_enum {
	_alu_and, _alu_eor,	_alu_sub, _alu_rsb,
	_alu_add, _alu_adc,	_alu_sbc, _alu_rsc,
//	_alu_tst, _alu_teq,	_alu_cmp, _alu_cmn,
	_alu_orr, _alu_mov, _alu_bic, _alu_mvn,
//
	__OPCODE_ALU0_COUNT,
}opcode_alu0_t;

typedef enum opcode_alu0_enum const opcode_alu0_eref;

typedef enum opcode_alu1_enum {
	_alu_clz,
	_alu_div,
	_alu_mac,
	_alu_mod,
	_alu_mul,
//
	_alu_sxtb, _alu_sxth, _alu_sxtr, _alu_sxtw,
	_alu_uxtb, _alu_uxth, _alu_uxtr, _alu_uxtw,
//
	__OPCODE_ALU1_COUNT,
}opcode_alu1_t;

typedef enum opcode_alu1_enum const opcode_alu1_eref;

typedef enum opcode_class_enum {
	_alu0, _alu0_s, _alu0_ea,
	_alu1, _alu1_s,
	_get_rm, _get_rn, _get_rs,
	_ldst,
	_misc,
	_skip_cc,
	_sop,
	_wb_rd, _wb_sop,
//
	__OPCODE_CLASS_COUNT,
}opcode_class_t;

typedef enum opcode_class_enum const opcode_class_eref;

typedef enum opcode_ldst_enum {
	_ldb, _ldsb, _stb,
	_ldh, _ldsh, _sth,
//	_ldq, _ldsq, _stq,
	_ldr, _ldsr, _str,
	_ldw, _ldsw, _stw,
}opcode_ldst_t;

typedef enum opcode_ldst_enum const opcode_ldst_eref;

typedef enum opcode_misc_enum {
	_misc_break, _misc_exit,
	_misc_get_rm_i16, _misc_get_rm_i32, _misc_get_rm_i8,
	_misc_get_rm_u8,
	_misc_get_rs_i8,
	_misc_wfi,
//
	__OPCODE_MISC_COUNT,
}opcode_misc_t;

typedef enum opcode_misc_enum const opcode_misc_eref;

typedef enum opcode_sop_enum {
	_sop_lsl, _sop_lsr, _sop_asr, _sop_ror, _sop_rrx,
	_alu_lsl = 8, _alu_lsr, _alu_asr, _alu_ror, _alu_rrx,
}opcode_sop_t;

typedef enum opcode_sop_enum const opcode_sop_eref;
