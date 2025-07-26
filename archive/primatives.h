#pragma once

/* **** */

#include <stdint.h>

/* **** */

#define _SOP(_name) \
	static inline uint32_t _##_name(const uint32_t rm, const uint8_t rs)

#define SOP(_name, _action) _SOP(_name) \
	{ return(_action); }

_SOP(asr) { return((int32_t)rm >> rs); }
SOP(lsl, rm << rs)
SOP(lsl_masked, _lsl(rm, rs & 31))
SOP(lsr, rm >> rs)
SOP(lsr_masked, _lsl(rm, rs & 31))
SOP(ror, _lsl_masked(rm, -rs) | _lsr(rm, rs))

/* **** */

#define _OP(_name) \
	static inline uint32_t _##_name(uint32_t *const rd, const uint32_t rn, const uint32_t sop)

#define OP(_name, _action) _OP(_name) \
	{ return(_action); }

OP(add, rn + sop)
OP(and, rn & sop)
OP(bext, BEXT(rn, sop))
OP(bic, rn & ~sop)
OP(div, rn / sop)
OP(eor, rn ^ sop)
OP(orr, rn | sop)
OP(mac, *rd + (rn * sop))
OP(mod, rn % sop)
_OP(mov) { return(sop); (void)rn; }
OP(mul, rn * sop)
_OP(mvn) { return(~sop); (void)rn; }
OP(nand, ~(_and(0, rn, sop)))
OP(nor, ~(_orr(0, rn, sop)))
OP(rsb, sop - rn)
OP(sub, rn - sop)

#define _INST(_name) \
	extern inline uint32_t _name(uint32_t *const rd, const uint32_t rn, const uint32_t rm, const uint8_t rs)

#define INST(_name) \
	_INST(_name##_asr) { return(_##_name(rd, rn, _asr(rm, rs))); } \
	_INST(_name##_lsl) { return(_##_name(rd, rn, _lsl(rm, rs))); } \
	_INST(_name##_lsr) { return(_##_name(rd, rn, _lsr(rm, rs))); } \
	_INST(_name##_ror) { return(_##_name(rd, rn, _ror(rm, rs))); }

#define INST_LIST0 \
	INST(add) \
	INST(and) \
	INST(bic) \
	INST(eor) \
	INST(mov) \
	INST(mvn) \
	INST(orr) \
	INST(rsb) \
	INST(sub) \
