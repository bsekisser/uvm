#include "uvm_mem.h"
#include "uvm.h"

/* **** */

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

/* **** */

#include <stddef.h>

/* **** */

static inline
int _mem_read(uvm_ref vm, unsigned long *const rd, const unsigned long ea, const size_t bytes)
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
int _mem_write(uvm_ref vm, const unsigned long rd, const unsigned long ea, const size_t bytes)
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

extern __attribute__((weak))
int mem_ifetch(void* param, unsigned long *const rd, unsigned long *const ea, const size_t bytes)
{ _mem_read(param, rd, *ea, bytes); *ea += bytes; return(0); }

extern __attribute__((weak))
int mem_read(void* param, unsigned long *const rd, const unsigned long ea, const size_t bytes)
{ _mem_read(param, rd, ea, bytes); return(0); }

extern __attribute__((weak))
int mem_write(void* param, unsigned const long rd, const unsigned long ea, const size_t bytes)
{ _mem_write(param, rd, ea, bytes); return(0); }
