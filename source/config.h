#define HOST
//#define BIT32

/* **** */


#ifdef HOST
	typedef long uvm_cell_st;
	typedef unsigned long uvm_cell_ut;
#else
	#include <stdint.h>

	#ifdef BIT32
		typedef int32_t uvm_cell_st;
		typedef uint32_t uvm_cell_ut;
	#endif
#endif

//#define ALWAYS_INLINE
#ifndef ALWAYS_INLINE
	#define ALWAYS_INLINE __attribute__((always_inline))
#endif

//#define INLINE
#ifndef INLINE
	#define INLINE inline
#endif

//#define STATIC extern
#ifndef STATIC
	#define STATIC static
#endif
