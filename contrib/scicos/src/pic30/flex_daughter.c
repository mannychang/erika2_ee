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
	@file flex_dmb.c
	@brief www.scicos.org, www.scicoslab.org
	@author Roberto Bucher, SUPSI- Lugano
	@author Simone Mannori, ScicosLab developer
	@date 2006-2010
*/ 
 
 
#include <machine.h>
#include <scicos_block4.h>

#include <ee.h>

#ifdef __USE_LCD__

#include "stdio.h"

// defined in the dspic_main template
extern int scicos_lcd_used;
extern char ee_lcd_line1[17];
extern char ee_lcd_line2[17];

void flex_daughter_lcd_init(void)
{
	scicos_lcd_used = 1;
}

void flex_daughter_lcd_end(void)
{
}

void EESCI_flexdmb_lcd_float_inout(float scicos_lcd_value1, float scicos_lcd_value2) {
	EE_pic30_disableIRQ();
	sprintf(ee_lcd_line1, "%+.6E", (double)scicos_lcd_value1);
	sprintf(ee_lcd_line2, "%+.6E", (double)scicos_lcd_value2);
	EE_pic30_enableIRQ();
}

void EESCI_flexdmb_lcd_uint8_inout(unsigned char* line1, unsigned char* line2) {
	int i;
	EE_pic30_disableIRQ();
	for(i=0; i<16; i++) {
		ee_lcd_line1[i] = line1[i];
		ee_lcd_line2[i] = line2[i];
	}
	ee_lcd_line1[16] = '\0';
	ee_lcd_line2[16] = '\0';
	EE_pic30_enableIRQ();
}

void flex_daughter_lcd_inout(int type, void* y_1, void* y_2)
{
	if (type == 1) {
		float* y1 = (float *)y_1;
		float* y2 = (float *)y_2;
		EESCI_flexdmb_lcd_float_inout(y1[0], y2[0]);
	}
	else {
		unsigned char* y1 = (unsigned char *)y_1;
		unsigned char* y2 = (unsigned char *)y_2;
		EESCI_flexdmb_lcd_uint8_inout(y1, y2 );
	}
}

#endif

#ifdef __USE_BUTTONS__

void flex_daughter_button_init()
{
	// Enable buttons without interrupt support
	EE_buttons_init(NULL, NULL);
}

void flex_daughter_button_end(void)
{
}

void flex_daughter_button_float_output(int val, void *ptr_y, int ptr_type)
{
	if(ptr_type == SCITYPE_FLOAT) {
		float *y = (float *)ptr_y;
		y[0] = val;
	}
	else if(ptr_type == SCITYPE_INT8) {
		char *y = (char *)ptr_y;
		y[0] = val;
	}
}

void flex_daughter_button_inout(int pin, void *ptr_y, int ptr_type)
{
	#if defined(__USE_DEMOBOARD__)
	if ((pin < 1) || (pin > 4))
	#elif defined(__USE_MOTIONBOARD__)
	if ((pin < 1) || (pin > 2))
	#endif
		return; //** return if outside the allowed range

	switch(pin) {
		case 1:
			if (EE_button_get_S1())
				flex_daughter_button_float_output(1, ptr_y, ptr_type);
			else
				flex_daughter_button_float_output(0, ptr_y, ptr_type);
			break;
		case 2:
			if (EE_button_get_S2())
				flex_daughter_button_float_output(1, ptr_y, ptr_type);
			else
				flex_daughter_button_float_output(0, ptr_y, ptr_type);
			break;
#if defined(__USE_DEMOBOARD__)
		case 3:
			if (EE_button_get_S3())
				flex_daughter_button_float_output(1, ptr_y, ptr_type);
			else
				flex_daughter_button_float_output(0, ptr_y, ptr_type);
			break;
		case 4:
			if (EE_button_get_S4())
				flex_daughter_button_float_output(1, ptr_y, ptr_type);
			else
				flex_daughter_button_float_output(0, ptr_y, ptr_type);
			break;
#endif // USE_DEMOBOARD__
	}
}

#endif


#ifdef __USE_LEDS__

#if defined(__USE_DEMOBOARD__)
#define FLEX_DAUGHTER_NUM_LEDS 8
#elif defined(__USE_MOTIONBOARD__)
#define FLEX_DAUGHTER_NUM_LEDS 2
#endif

void flex_daughter_leds_barrier_init(void)
{
	EE_daughter_leds_init();
}

void flex_daughter_leds_barrier_inout(float threshold, float *leds_values)
{
	int i;
	
	if( threshold < 0.01 || threshold > 0.99 )
		threshold = 0.5;
		
	for(i=FLEX_DAUGHTER_NUM_LEDS-1; i>=0; i--) {
		if(leds_values[i] > threshold) {
			switch (i) { //** set the bit to one 
				case 1:
					EE_led_0_on(); break;
				case 2:
					EE_led_1_on(); break;
				#if defined(__USE_DEMOBOARD__)
				case 3:
					EE_led_2_on(); break;
				case 4:
					EE_led_3_on(); break;
				case 5:
					EE_led_4_on(); break;
				case 6:
					EE_led_5_on(); break;
				case 7:
					EE_led_6_on(); break;
				case 8:
					EE_led_7_on(); break;
				#endif
			}
		} else {
			switch (i) { //** set the bit to zero 
				case 1:
					EE_led_0_off(); break;
				case 2:
					EE_led_1_off(); break;
				#if defined(__USE_DEMOBOARD__)
				case 3:
					EE_led_2_off(); break;
				case 4:
					EE_led_3_off(); break;
				case 5:
					EE_led_4_off(); break;
				case 6:
					EE_led_5_off(); break;
				case 7:
					EE_led_6_off(); break;
				case 8:
					EE_led_7_off(); break;
				#endif
			}
		}
	}
}

void flex_daughter_leds_barrier_end(void)
{
	EE_leds(0xFF); 
}

void flex_daughter_leds_init(void)
{
	flex_daughter_leds_barrier_init();
}

void flex_daughter_leds_inout(float threshold, float *leds_values)
{
	flex_daughter_leds_barrier_inout(threshold, leds_values);
}

void flex_daughter_leds_end(void)
{
	flex_daughter_leds_barrier_end();
}

#endif





