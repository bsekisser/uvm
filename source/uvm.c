#include "uvm.h"

/* **** */

#include "uvm_alubox.h"
#include "uvm_cc.h"
//#include "uvm_flagbox.h"
#include "uvm_ldst.h"
#include "uvm_mem.h"
#include "uvm_shiftbox.h"

/* **** */

#include "git/libbse/include/log.h"

/* **** */

#include <assert.h>
#include <stdlib.h>

/* **** */

static __attribute__((constructor))
void preflight_checks(void)
{
	assert(16 > __OPCODE_ALU0_COUNT);
	assert(16 > __OPCODE_ALU1_COUNT);
	assert(16 > __OPCODE_CLASS_COUNT);
}

// shift_operand_shift_immediate(shift_type, rm, rs)
	//	_getR(rm), _get8u(rs), _sop(shift_type)
// shift_operand_shift_register(shift_type, rm, rs)
	//	_getR(rm), _getR(rs), _sop(shift_type)

// dp_immediate(opcode, rd, rn, rm, rs)
	//	(skip_cc(cc)), shift_operand, (_getR(rn)), _alu(opcode), (_wb(rd)), 0

// ldst_shift_immediate(opcode, rd, rn, shift_type(rm, rs))
	//	(skip_cc(cc)), shift_operand, _getR(n), _alu(opcode), (_wb(ea)), (_wb(rd)), 0

static inline
int uvm_step_ucode(uvm_ref vm, uvm_pc_t upc)
{
	uvm_cell_ut rd = 0, rn = 0, rm = 0, rs = 0;
	uvm_cell_ut ea = 0, sop = 0;

	for(;;) {
		const char uir = *upc.p2u8++;
		if(0 ==uir) return(0);
		
		const opcode_class_t opclass = (uir >> 4) & 15;
		const unsigned opsubop = uir & 15;

		CYCLE++;

		uvm_cell_upref p2r = &GPRx(uir);

		switch(opclass) {
			case _alu0: rd = _alubox0(vm, opsubop, rn, sop); break;
			case _alu0_s: rd = _alubox0s(vm, opsubop, rn, sop); break;
			case _alu0_ea: ea = _alubox0(vm, opsubop, rn, sop); break;
			case _alu1: rd = _alubox1(vm, opsubop, rn, sop, rm, rs); break;
			case _alu1_s: rd = _alubox1s(vm, opsubop, rn, sop, rm, rs); break;
			case _get_rm: rm = *p2r; break;
			case _get_rn: rn = *p2r; break;
			case _get_rs: rs = *p2r; break;
			case _ldst: _ldstbox(vm, opsubop, &rd, ea); break;
			case _misc: switch((opcode_misc_eref)opsubop) {
				case _misc_break: return(*upc.p2i8);
				case _misc_exit: exit(*upc.p2i8); break;
				case _misc_get_rm_i16: rm = *upc.p2i16++; break;
				case _misc_get_rm_i32: rm = *upc.p2i32++; break;
				case _misc_get_rm_i8: rm = *upc.p2i8++; break;
				case _misc_get_rm_u8: rm = *upc.p2u8++; break;
				case _misc_get_rs_i8: rs = *upc.p2i8++; break;
				case _misc_wfi: vm->cpsr.wfi = 1; return(0);
				default: break;
			}break;
			case _skip_cc: if(_check_condition_code(vm, opsubop)) continue; else return(0);
			case _sop: {
				uvm_cell_upref r = (opsubop & 8) ? &sop : & rd;
				*r = _shiftbox(vm, opsubop, rm, rs);
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

int uvm_step(uvm_ref vm)
{
	IP = PC;
	mem_ifetch(vm, &IR, &PC, 1);
	
	uvm_pc_t upc = { .p = &vm->ucode[IR & 255][0] };

	return(uvm_step_ucode(vm, upc));
}
