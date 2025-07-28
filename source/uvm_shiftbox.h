#pragma once

/* **** */

#include "uvm.h"
#include "uvm_opcode.h"

/* **** */

#define kSHIFT_BITS (sizeof(uvm_cell_ut) << 3)
#define kSHIFT_MASK (kSHIFT_BITS - 1)

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _asr(uvm_cell_stref rm, const char rs) { return(rm >> rs); }

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _lsl(uvm_cell_utref rm, const char rs) { return(rm << rs); }

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _lsl_masked(uvm_cell_utref rm, const char rs) { return(_lsl(rm, rs & kSHIFT_MASK)); }

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _lsr(uvm_cell_utref rm, const char rs) { return(rm >> rs); }

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _lsr_masked(uvm_cell_utref rm, const char rs) { return(_lsr(rm, rs & kSHIFT_MASK)); }

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _ror(uvm_cell_utref rm, const char rs) { return(_lsl_masked(rm, -rs) | _lsr(rm, rs)); }

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _rrx(uvm_ref vm, uvm_cell_utref rm)
{
	uvm_cell_utref sop = (_lsl(APSR(c), kSHIFT_MASK) | _lsr(rm, 1));
	APSR(c) = rm & 1;
	
	return(sop);
}

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _shiftbox(uvm_ref vm, opcode_sop_eref opcode, uvm_cell_utref rm, const unsigned char rs)
{
	switch(opcode) {
		case _alu_asr: case _sop_asr: return(_asr(rm, rs));
		case _alu_lsl: case _sop_lsl: return(_lsl(rm, rs));
		case _alu_lsr: case _sop_lsr: return(_lsr(rm, rs));
		case _alu_ror: case _sop_ror: return(_ror(rm, rs));
		case _alu_rrx: case _sop_rrx: return(_rrx(vm, rm));
	}
	
	return(rm);
}
