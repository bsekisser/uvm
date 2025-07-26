#include <stdint.h>

typedef struct alu_tag* alu_ptr;
typedef alu_ptr const alu_ref;

typedef struct alu_tag {
	uint32_t gpr[16];

	struct {
		unsigned cf:1;
	}cpsr;
}alu_t;

alu_ptr alu;

#include "primatives.h"

#define _OP16(_name) \
	static inline uint32_t _##_name(uint32_t *const rd, const int16_t rn, const int16_t sop)

#define OP16(_name, _action) _OP16(_name) \
	{ return(_action); }

extern inline
uint32_t __flags_add(const uint32_t rn, const uint32_t sop, const unsigned cf)
{ return(rn + sop + cf ? alu->cpsr.cf : 0); }

extern inline
uint32_t __flags_sub(const uint32_t rn, const uint32_t sop, const unsigned cf)
{ return(rn - sop - cf ? alu->cpsr.cf : 0); }

OP(cmn, __flags_add(rn, sop, 0))
OP(cmp, __flags_sub(rn, sop, 0))
OP(teq, _eor(rd, rn, sop))
OP(tst, _and(rd, rn, sop))

OP(adc, _add(rd, rn, sop + alu->cpsr.cf))
OP(rsc, _rsb(rd, sop + alu->cpsr.cf, rn))
OP(sbc, _sub(rd, rn, sop + alu->cpsr.cf))

#define INST_LIST1 \
	INST(adc) \
	INST(bext) \
	INST(div) \
	INST(mac) \
	INST(mod) \
	INST(mul) \
	INST(nand) \
	INST(nor) \
	INST(rsc) \
	INST(sbc)

#define INST_LIST2 \
	INST(cmn) \
	INST(cmp) \
	INST(teq) \
	INST(tst)

INST_LIST0

#undef INST
#define INST(_name) \
	_INST(_name) { return(_##_name(rd, rn, rm)); } \

INST_LIST1
INST_LIST2

typedef enum arm_aluop_enum {
	_arm_and, _arm_eor, _arm_sub, _arm_rsb,
	_arm_add, _arm_adc, _arm_sbc, _arm_rsc,
	_arm_tst, _arm_teq, _arm_cmp, _arm_cmn,
	_arm_orr, _arm_mov, _arm_bic, _arm_mvn,
//
	_arm_div, _arm_mac, _arm_mod, _arm_mul,
//
	_arm_bext, _arm_nand, _arm_nor,
}arm_aluop_t;

typedef enum arm_sop_enum {
	_sop_lsl, _sop_lsr, _sop_asr, _sop_ror,
}arm_sop_t;

#define SOP_LIST \
	SOP(asr) \
	SOP(lsl) \
	SOP(lsr) \
	SOP(ror)

extern inline
uint32_t _shiftbox(const arm_sop_t sop, const uint32_t rm, const uint8_t rs)
{
#undef SOP
#define SOP(_name) case _sop_##_name: return(_##_name(rm, rs));

	switch(sop) {
		SOP_LIST
	}

	return(rm);
}

extern inline
uint32_t _alubox(const arm_aluop_t opcode, uint32_t *const rd, const uint32_t rn, const uint32_t sop)
{
#undef INST
#define INST(_name) \
	case _arm_##_name: return(_##_name(rd, rn, sop)); \

	switch(opcode) {
		INST_LIST0
//		default: switch(opcode >> 4) {
//			case 0x80: switch(opcode & 4) {
					INST_LIST1
//			}break;
//			case 0x90: switch(opcode & 4) {
					INST_LIST2
//			}break;
//		}
	}
	
	return(sop);
}

extern inline
uint32_t _alubox_shift(const unsigned opcode, uint32_t *const rd, const uint32_t rn, const uint32_t rm, const uint8_t rs)
{
#undef INST
#define INST(_name) \
	case ((_sop_asr << 4) | _arm_##_name): return(_name##_asr(rd, rn, rm, rs)); \
	case ((_sop_lsl << 4) | _arm_##_name): return(_name##_lsl(rd, rn, rm, rs)); \
	case ((_sop_lsr << 4) | _arm_##_name): return(_name##_lsr(rd, rn, rm, rs)); \
	case ((_sop_ror << 4) | _arm_##_name): return(_name##_ror(rd, rn, rm, rs));

	switch(opcode) {
		INST_LIST0
	}

	return(rm);
}

extern inline
uint32_t _alubox_shiftbox(const arm_aluop_t opcode, const arm_sop_t sop, uint32_t *const rd, const uint32_t rn, const uint32_t rm, const uint8_t rs)
{ return(_alubox(opcode, rd, rn, _shiftbox(sop, rm, rs))); }
