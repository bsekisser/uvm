#include "uvm.h"

/* **** */

#include "git/libarm/include/arm_ir.h"

#if defined(HAVE_INT64_T) || defined(__aarch64__)
	#include "git/libbse/include/generic_data_64.h"
	#define IF_USE64(_) _
#else
	#define IF_USE64(_)
#endif

#if defined(HAVE_INT128_T) // defined(__aarch64__)
	#include "git/libbse/include/generic_data_128.h"
	#define IF_USE128(_) _
#else
	#define IF_USE128(_)
#endif

#include "git/libbse/include/generic_data.h"
#include "git/libbse/include/log.h"

#include "git/libbse/include/shift_roll.h"

/* **** */

#include <stdint.h>
#include <stdlib.h>

/* **** */

//#define STATIC extern
#ifndef STATIC
	#define STATIC static
#endif

//#define INLINE
#ifndef INLINE
	#define INLINE inline
#endif

/* **** */

static inline
int _mem_read(vm_ref vm, unsigned long *const rd, const unsigned long ea, const size_t bytes)
{
	generic_data_t src = { .p = vm->mem.data + ea };
	IF_USE64(generic64_data_t src64 = { .p = src.p };)
	IF_USE128(generic128_data_t src128 = { .p = src.p };)

	switch(bytes) {
		IF_USE128(case 16: *rd = *src128.p2u128; break;)
		IF_USE64(case 8: *rd = *src64.p2u64; break;)
		case 4: *rd = *src.p2u32; break;
		case 2: *rd = *src.p2u16; break;
		case 1: *rd = *src.p2u8; break;
		default:
			return(-1);
	}
	
	return(0);
}

static inline
int _mem_write(vm_ref vm, const unsigned long rd, const unsigned long ea, const size_t bytes)
{
	generic_data_t dst = { .p = vm->mem.data + ea };
	IF_USE64(generic64_data_t dst64 = { .p = dst.p };)
	IF_USE128(generic128_data_t dst128 = { .p = dst.p };)

	switch(bytes) {
		IF_USE128(case 16: *dst128.p2u128 = rd; break;)
		IF_USE64(case 8: *dst64.p2u64 = rd; break;)
		case 4: *dst.p2u32 = rd; break;
		case 2: *dst.p2u16 = rd; break;
		case 1: *dst.p2u8 = rd; break;
		default:
			return(-1);
	}

	return(0);
}

/* **** */

__attribute__((weak))
int ifetch(void* param, unsigned long *const rd, unsigned long *const ea, const size_t bytes)
{ _mem_read(param, rd, *ea, bytes); *ea += bytes; return(0); }

__attribute__((weak))
int mem_read(void* param, unsigned long *const rd, const unsigned long ea, const size_t bytes)
{ _mem_read(param, rd, ea, bytes); return(0); }

__attribute__((weak))
int mem_write(void* param, unsigned const long rd, const unsigned long ea, const size_t bytes)
{ _mem_write(param, rd, ea, bytes); return(0); }

/* **** */

STATIC INLINE
int _check_condition_code(vm_ref vm, const opcode_condition_t cc)
{
	const unsigned cf = vm->cpsr.cf;
	const unsigned nf = vm->cpsr.nf;
	const unsigned zf = vm->cpsr.zf;
	const unsigned vf = vm->cpsr.vf;
	const unsigned nfvf = (nf == vf);
	
	switch(cc) {
		case _cc_eq: return(zf);
		case _cc_ne: return(!zf);
		case _cc_cs: return(cf);
		case _cc_cc: return(!cf);
		case _cc_mi: return(nf);
		case _cc_pl: return(!nf);
		case _cc_vs: return(vf);
		case _cc_vc: return(!vf);
		case _cc_hi: return(cf && !zf);
		case _cc_ls: return(!cf || zf);
		case _cc_ge: return(nfvf);
		case _cc_lt: return(!nfvf);
		case _cc_gt: return(!zf && nfvf);
		case _cc_le: return(zf || !nfvf);
		case _cc_al: return(1);
		default: break;
	}
	
	return(0);
}

STATIC INLINE
unsigned long _alubox_flags_nz(vm_ref vm, const unsigned s, const long rd)
{
	if(s) {
		vm->cpsr.nf = (0 > (int32_t)rd);
		vm->cpsr.zf = (0 == rd);
	}

	return(rd);
}

/*
 * Credit to:
 * 		http://www.emulators.com/docs/nx11_flags.htm
 *
 * OF(A+B) = ((A XOR D) AND NOT (A XOR B)) < 0
 * OF(A-B) = ((A XOR D) AND (A XOR B)) < 0
 *
 * CF(A+B) = (((A XOR B) XOR D) < 0) XOR (((A XOR D) AND NOT (A XOR B)) < 0)
 * CF(A-B) = (((A XOR B) XOR D) < 0) XOR (((A XOR D) AND (A XOR B)) < 0)
 *
 */

STATIC INLINE
unsigned long _alubox_flags__add_sub(vm_ref vm, const unsigned s, const unsigned long rn, const unsigned long sop, const unsigned carry_in, const unsigned is_sub)
{
	const unsigned long rd = rn + sop + carry_in;
	
	if(s) {
		const unsigned xvec = (rn ^ (sop + carry_in));
		const long ovec = (rn ^ rd) & (is_sub ? xvec : ~xvec);
		const long cvec = (xvec ^ ovec ^ rd);
		
		vm->cpsr.cf = (0 > (int32_t)cvec);
		vm->cpsr.vf = (0 > (int32_t)ovec);
	}

	return(rd);
}

STATIC INLINE
unsigned long _alubox_flags_add(vm_ref vm, const unsigned s,
	const unsigned long rn, const unsigned long sop, const unsigned carry_in)
{ return(_alubox_flags__add_sub(vm, s, rn, sop, carry_in, 0)); }

STATIC INLINE
unsigned long _alubox_flags_sub(vm_ref vm, const unsigned s,
	const unsigned long rn, const unsigned long sop, const unsigned carry_in)
{ return(_alubox_flags__add_sub(vm, s, rn, sop, carry_in, 1)); }

STATIC INLINE
unsigned long _alubox(vm_ref vm, opcode_alu_t opcode, const unsigned s,
	const unsigned long rn, const unsigned long sop)
{
	const unsigned cf = vm->cpsr.cf;
	uint32_t rd = 0;

	switch(opcode) {
		case _alu_adc:
			rd = _alubox_flags_add(vm, s, rn, sop, cf);
				break;
		case _alu_add: case _alu_cmn:
			rd = _alubox_flags_add(vm, s, rn, sop, 0);
				break;
		case _alu_and: case _alu_tst:
			rd = rn & sop;
				break;
		case _alu_bic:
			rd = rn & ~sop;
				break;
		case _alu_clz:
			rd = __builtin_clz(sop);
				break;
		case _alu_cmp: case _alu_sub:
			rd = _alubox_flags_sub(vm, s, rn, sop, 0);
				break;
		case _alu_div:
			rd = (sop ? (rn / sop) : 0);
				break;
		case _alu_eor: case _alu_teq:
			rd = rn ^ sop;
				break;
		case _alu_mod:
			rd = (sop ? (rn % sop) : 0);
				break;
		case _alu_mov:
			rd = sop;
				break;
		case _alu_mul:
			rd = rn * sop;
				break;
		case _alu_mvn:
			rd = ~sop;
				break;
		case _alu_orr:
			rd = rn | sop;
				break;
		case _alu_rsb:
			rd = _alubox_flags_sub(vm, s, sop, rn, 0);
				break;
		case _alu_rsc:
			rd = _alubox_flags_sub(vm, s, sop, rn, cf);
				break;
		case _alu_sbc:
			rd = _alubox_flags_sub(vm, s, rn, sop, cf);
				break;
	}

	if(s)
		_alubox_flags_nz(vm, s, rd);

	return(rd);
}

STATIC INLINE
void _ldstbox(vm_ref vm, opcode_ldst_t opcode, unsigned long *const rd, const unsigned long ea)
{
	switch(opcode) {
		case _ldb: case _ldsb: mem_read(vm, rd, ea, 1); break;
		case _ldh: case _ldsh: mem_read(vm, rd, ea, 2); break;
		case _ldr: case _ldsr: mem_read(vm, rd, ea, 4); break;
		case _stb: mem_write(vm, *rd, ea, 1); break;
		case _sth: mem_write(vm, *rd, ea, 2); break;
		case _str: mem_write(vm, *rd, ea, 4); break;
	}
	
	switch(opcode) {
		case _ldsb: *rd = (int8_t)*rd; break;
		case _ldsh: *rd = (int16_t)*rd; break;
		case _ldsr: *rd = (int32_t)*rd; break;
		default: break;
	}
}

STATIC INLINE
unsigned long _shiftbox(vm_ref vm, opcode_sop_t opcode, const unsigned long rm, const unsigned char rs)
{
	switch(opcode) {
		case _alu_asr: case _sop_asr: return(_asr(rm, rs));
		case _alu_lsl: case _sop_lsl: return(_lsl(rm, rs));
		case _alu_lsr: case _sop_lsr: return(_lsr(rm, rs));
		case _alu_ror: case _sop_ror: return(_ror(rm, rs));
		case _alu_rrx: case _sop_rrx: return(_rrx_v(rm, vm->cpsr.cf));
	}
	
	return(rm);
}

/* **** */

// shift_operand_shift_immediate(shift_type, rm, rs)
	//	_getR(rm), _get8u(rs), _sop(shift_type)
// shift_operand_shift_register(shift_type, rm, rs)
	//	_getR(rm), _getR(rs), _sop(shift_type)

// dp_immediate(opcode, rd, rn, rm, rs)
	//	(skip_cc(cc)), shift_operand, (_getR(rn)), _alu(opcode), (_wb(rd)), 0

// ldst_shift_immediate(opcode, rd, rn, shift_type(rm, rs))
	//	(skip_cc(cc)), shift_operand, _getR(n), _alu(opcode), (_wb(ea)), (_wb(rd)), 0

static inline
int uvm_step_ucode(vm_ref vm, uvm_pc_t upc)
{
	unsigned long rd = 0, rn = 0, rm = 0, rs = 0;
	unsigned long ea = 0, sop = 0;

	for(;;) {
		const char uir = *upc.p2u8++;
		if(0 ==uir) return(0);
		
		const opcode_class_t opclass = (uir >> 4) & 15;
		const unsigned opsubop = uir & 15;

		CYCLE++;

		unsigned long *const p2r = &GPRx(uir);

		switch(opclass) {
			case _alu: rd = _alubox(vm, opsubop, rn, sop, 0); break;
			case _alu_s: rd = _alubox(vm, opsubop, rn, sop, 1); break;
			case _alu_ea: ea = _alubox(vm, opsubop, rn, sop, 0); break;
			case _get_rm: rm = *p2r; break;
			case _get_rn: rn = *p2r; break;
			case _get_rs: rs = *p2r; break;
			case _ldst: _ldstbox(vm, opsubop, &rd, ea); break;
			case _misc: switch((opcode_misc_t)opsubop) {
				case _misc_break: return(*upc.p2i8);
				case _misc_exit: exit(*upc.p2i8); break;
				case _misc_get_rm_i16: rm = *upc.p2i16++; break;
				case _misc_get_rm_i32: rm = *upc.p2i32++; break;
				case _misc_get_rm_i8: rm = *upc.p2i8++; break;
				case _misc_get_rm_u8: rm = *upc.p2u8++; break;
				case _misc_get_rs_i8: rs = *upc.p2i8++; break;
			}break;
			case _skip_cc: if(_check_condition_code(vm, opsubop)) continue; else return(0);
			case _sop: switch((opcode_sop_t)opsubop) {
				case _alu_asr: case _alu_lsl: case _alu_lsr: case _alu_ror: case _alu_rrx:
					rd = _shiftbox(vm, opsubop, rm, rs); break;
				case _sop_asr: case _sop_lsl: case _sop_lsr: case _sop_ror: case _sop_rrx:
					sop = _shiftbox(vm, opsubop, rm, rs); break;
			}break;
			case _wb_rd: *p2r = rd; break;
			case _wb_sop: *p2r = sop; break;
			case __OPCODE_CLASS_COUNT: break;
				LOG_ACTION(exit(-1));
		}
	}

	ICOUNT++;
	return(0);
}

int uvm_step(vm_ref vm)
{
	IP = PC;
	ifetch(vm, &IR, &PC, 4);
	
	uvm_pc_t upc = { .p = &vm->ucode[IR & 255][0] };

	return(uvm_step_ucode(vm, upc));
}
