#ifndef __mem_compiler_h__
#define __mem_compiler_h__

/**
* Start Compiler Selection section
* \todo What is the global symbol for Erika Enterprise Kernel?
*/
#ifdef __ERIKA__		/* Erika Abstract Compiler */
#include <ee.h>
#include <arch/mem_compiler_ee.h>

#elif defined __C30__ 	/* Microchip C30 */
#include <arch/mem_compiler_c30.h>

#else			/* No compiler */
#error "MEMORY LIBRARY ERROR: compiler type not specified"

#endif	/* End Compiler Selection */

#endif /* Header Protection */
