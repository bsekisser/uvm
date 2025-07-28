#pragma once

/* **** */

#include "uvm_mem.h"
#include "uvm_opcode.h"
#include "uvm.h"

/* **** */

STATIC
void _ldstbox(uvm_ref vm, opcode_ldst_eref opcode, uvm_cell_upref rd, uvm_cell_utref ea)
{
	switch(opcode) {
		case _ldb: case _ldsb: mem_read(vm, rd, ea, 1); break;
		case _ldh: case _ldsh: mem_read(vm, rd, ea, 2); break;
		case _ldr: case _ldsr: mem_read(vm, rd, ea, sizeof(uvm_cell_ut)); break;
		case _ldw: case _ldsw: mem_read(vm, rd, ea, 4); break;
		case _stb: mem_write(vm, *rd, ea, 1); break;
		case _sth: mem_write(vm, *rd, ea, 2); break;
		case _str: mem_write(vm, *rd, ea, sizeof(uvm_cell_ut)); break;
		case _stw: mem_write(vm, *rd, ea, 4); break;
	}
	
	switch(opcode) {
		case _ldsb: *rd = (int8_t)*rd; break;
		case _ldsh: *rd = (int16_t)*rd; break;
		case _ldsr: *rd = (uvm_cell_st)*rd; break;
		case _ldsw: *rd = (int32_t)*rd; break;
		default: break;
	}
}
