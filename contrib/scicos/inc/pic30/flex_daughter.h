/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2006-2010  Simone Mannori, Roberto Bucher
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

/** 
	@file flex_dmb.h
	@brief www.scicos.org, www.scicoslab.org
	@author Roberto Bucher, SUPSI- Lugano
	@author Simone Mannori, ScicosLab developer
	@date 2006-2010
*/ 
 
 
#include <machine.h>
#include <scicos_block4.h>

#include <ee.h>

#ifdef __USE_LCD__
void flex_daughter_lcd_init(void);
void flex_daughter_lcd_end(void);
void EESCI_flexdmb_lcd_float_inout(float scicos_lcd_value1, float scicos_lcd_value2);
void EESCI_flexdmb_lcd_uint8_inout(unsigned char* line1, unsigned char* line2);
void flex_daughter_lcd_inout(int type, void* y_1, void* y_2);
#endif

#ifdef __USE_BUTTONS__
void flex_daughter_button_init();
void flex_daughter_button_end(void);
void flex_daughter_button_float_output(int val, void *ptr_y, int ptr_type);
void flex_daughter_button_inout(int pin, void *ptr_y, int ptr_type);
#endif

#ifdef __USE_LEDS__
void flex_daughter_leds_barrier_init(void);
void flex_daughter_leds_barrier_inout(float threshold, float *leds_values);
void flex_daughter_leds_barrier_end(void);
void flex_daughter_leds_init(void);
void flex_daughter_leds_inout(float threshold, float *leds_values);
void flex_daughter_leds_end(void);
#endif
