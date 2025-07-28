#pragma once

/* **** */

#include "uvm_alu_flags.h"
#include "uvm_opcode.h"
#include "uvm.h"

/* **** */

#include "git/libbse/include/unused.h"

/* **** */

#define __ALU(_lname) _##_lname

#define _ALU0(_lname) \
	STATIC INLINE ALWAYS_INLINE \
	uvm_cell_ut __ALU(_lname)(uvm_ref vm, uvm_cell_utref rn, uvm_cell_utref sop)

#define _ALU1(_lname) \
	STATIC INLINE ALWAYS_INLINE \
	uvm_cell_ut __ALU(_lname)(uvm_ref vm, uvm_cell_utref rn, uvm_cell_utref rm, uvm_cell_utref rs)

#define ALU0(_lname, _uname, _action) _ALU0(_lname) { return(_action); UNUSED(rn, vm); }
#define ALU1(_lname, _uname, _action) _ALU1(_lname) { return(_action); UNUSED(rm, rn, rs, vm); }

#define ALU_LIST0(_) \
	_(adc, ADC, rn + sop + APSR(c)) \
	_(add, ADD, rn + sop) \
	_(and, AND, rn & sop) \
	_(bic, BIC, rn & ~sop) \
	_(eor, EOR, rn ^ sop) \
	_(mov, MOV, sop) \
	_(mvn, MVN, ~sop) \
	_(orr, ORR, rn | sop) \
	_(rsb, RSB, sop - rn) \
	_(rsc, RSC, sop - rn - APSR(c)) \
	_(sbc, SBC, rn - sop - APSR(c)) \
	_(sub, SUB, rn - sop) \

#define ALU_LIST1a(_) \
	_(clz, CLZ, __builtin_clz(sop)) \
	_(div, DIV, (sop ? (rn / sop) : 0)) \
	_(mod, MOD, (sop ? (rn % sop) : 0)) \
	_(mul, MUL, rn * sop) \
	_(sxtb, SXTB, (int8_t)sop) \
	_(sxth, SXTH, (int16_t)sop) \
	_(sxtr, SXTR, (uvm_cell_st)sop) \
	_(sxtw, SXTH, (int32_t)sop) \
	_(uxtb, UXTB, (uint8_t)sop) \
	_(uxth, UXTH, (uint16_t)sop) \
	_(uxtr, UXTR, (uvm_cell_ut)sop) \
	_(uxtw, UXTW, (uint32_t)sop) \
	
#define ALU_LIST1b(_) \
	_(mac, MAC, rn + rm * rs)

ALU_LIST0(ALU0)
ALU_LIST1a(ALU0)
ALU_LIST1b(ALU1)

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _alubox0(uvm_ref vm, opcode_alu0_eref opcode,
	uvm_cell_utref rn, uvm_cell_utref sop)
{
#define _ALU_ESAC0(_lname) _alu_##_lname
#define ALU_ESAC0(_lname, _uname, _action) case _ALU_ESAC0(_lname): return(_##_lname(vm, rn, sop));

	switch(opcode) {
		ALU_LIST0(ALU_ESAC0)
		default: break;
	}

	return(sop);
}

STATIC
uvm_cell_ut _alubox0s(uvm_ref vm, opcode_alu0_eref opcode,
	uvm_cell_utref rn, uvm_cell_utref sop)
{
	uvm_cell_ut rd = _alubox0(vm, opcode, rn, sop);

	switch(opcode) {
		case _ALU_ESAC0(adc): case _ALU_ESAC0(add):
			_alubox_flags_add(vm, rd, rn, sop);
			break;
		case _ALU_ESAC0(rsb): case _ALU_ESAC0(rsc):
			_alubox_flags_sub(vm, rd, sop, rn);
			break;
		case _ALU_ESAC0(sbc): case _ALU_ESAC0(sub):
			_alubox_flags_sub(vm, rd, rn, sop);
			break;
		default: break;
	}

	return(_alubox_flags_nz(vm, rd));
}

STATIC INLINE ALWAYS_INLINE
uvm_cell_ut _alubox1(uvm_ref vm, opcode_alu1_eref opcode,
	uvm_cell_utref rn, uvm_cell_utref sop, uvm_cell_utref rm, uvm_cell_utref rs)
{
	uvm_cell_ut rd = 0;

#define _ALU_ESAC1(_lname) _alu_##_lname
#define ALU_ESAC1(_lname, _uname, _action) case _ALU_ESAC1(_lname): rd = _##_lname(vm, rn, rm, rs); break;

	switch(opcode) {
		ALU_LIST1a(ALU_ESAC0)
		ALU_LIST1b(ALU_ESAC1)
		default: break;
	}
	return(rd);
}

STATIC
uvm_cell_ut _alubox1s(uvm_ref vm, opcode_alu1_eref opcode,
	uvm_cell_utref rn, uvm_cell_utref sop, uvm_cell_utref rm, uvm_cell_utref rs)
{ return(_alubox_flags_nz(vm, _alubox1(vm, opcode, rn, sop, rm, rs))); }
