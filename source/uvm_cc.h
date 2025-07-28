#pragma once

/* **** */

#include "uvm.h"

/* **** */

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

typedef enum opcode_condition_enum const opcode_condition_eref;

#define CC_LIST(_) \
	_(eq, EQ, APSR(z)) \
	_(ne, NE, !APSR(z)) \
	_(cs, CS, APSR(c)) \
	_(cc, CC, !APSR(c)) \
	_(mi, MI, APSR(n)) \
	_(pl, PL, !APSR(n)) \
	_(vs, VS, APSR(v)) \
	_(vc, VC, !APSR(v)) \
	_(hi, HI, APSR(c) && !APSR(z)) \
	_(ls, LS, !APSR(c) || APSR(z)) \
	_(ge, GE, APSR(n) == APSR(v)) \
	_(lt, LT, !(APSR(n) == APSR(v))) \
	_(gt, GT, !APSR(z) && (APSR(n) == APSR(v))) \
	_(le, LE, APSR(z) || !(APSR(n) == APSR(v))) \
	_(al, AL, 1) \
	_(nv, AL, 0) \

#define _CC_NAME(_name) _cc_##_name

STATIC
int _check_condition_code(uvm_ref vm, const opcode_condition_eref cc)
{
#define CC_ESAC(_lname, _uname, _test) case _CC_NAME(_lname): return(_test); break;
	switch(cc) {
		CC_LIST(CC_ESAC)
		default: break;
	}
	
	return(0);
}
