#pragma once

/* **** */

#include "uvm.h"

/* **** */

STATIC
uvm_cell_ut _alubox_flags_nz(uvm_ref vm, uvm_cell_stref rd)
{
	APSR(n) = (0 > rd);
	APSR(z) = (0 == rd);
	
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

STATIC
uvm_cell_ut _alubox_flags__add_sub(uvm_ref vm,
	uvm_cell_utref rd, uvm_cell_utref rn, uvm_cell_utref sop,
	const unsigned is_sub)
{
	uvm_cell_utref xvec = (rn ^ sop);
	uvm_cell_stref ovec = (rn ^ rd) & (is_sub ? xvec : ~xvec);
	uvm_cell_stref cvec = (xvec ^ ovec ^ rd);
		
	APSR(c) = (0 > cvec);
	APSR(v) = (0 > ovec);
	
	return(rd);
}

STATIC
uvm_cell_ut _alubox_flags_add(uvm_ref vm,
	uvm_cell_utref rd, uvm_cell_utref rn, uvm_cell_utref sop)
{ return(_alubox_flags__add_sub(vm, rd, rn, sop, 0)); }

STATIC
uvm_cell_ut _alubox_flags_sub(uvm_ref vm,
	uvm_cell_utref rd, uvm_cell_utref rn, uvm_cell_utref sop)
{ return(_alubox_flags__add_sub(vm, rd, rn, sop, 1)); }
