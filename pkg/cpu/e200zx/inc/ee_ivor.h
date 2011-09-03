/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */
/*
 * Interrupt Vector Offset Registers and setup
 * Author: 2011 Bernardo  Dal Seno
 */


#ifndef __EE_CPU_E200ZX_IVOR_H__
#define __EE_CPU_E200ZX_IVOR_H__

#ifdef __ASM_CONVERTED_TO_C__
/* Prototypes are included only for Erika source files, but they could be useful
 * also for user C code.  In that case, a further condition should be added. */
#include <cpu/e200zx/inc/ee_irq_internal.h>
void save_registers(void);
void restore_all_return(void);
void ivor_setup(void);
void EE_e200zx_external_input_handler(void);
void EE_e200zx_decrementer_handler(void);
void EE_e200zx_fixed_intv_handler(void);
void cpu_setup(void);
void mmu_setup(void);
void sram_setup(void);
#endif

#define handler_prologue	bl	save_registers

#define handler_epilogue	b	restore_all_return

#endif /* __EE_CPU_E200ZX_IVOR_H__ */
