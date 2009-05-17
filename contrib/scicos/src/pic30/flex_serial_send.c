/* ###*B*###
 * Copyright (C) Roberto Bucher (SUPSI- Lugano)
 *               Simone Mannori (Scilab / INRIA / DIGITEO)
 *
 * Copyright (C) METALAU Project (INRIA)
 * ###*E*### */
 
 
#include <machine.h>
#include <scicos_block4.h>

#include <ee.h>
#include "mcu/microchip_dspic/inc/ee_uart.h"


void flex_serial_send(scicos_block *block,int flag)
{
	EE_UINT8 serial_port = block->ipar[0];
	int baudrate = block->ipar[1];
  float * u = block->inptr[0];

	if ((serial_port < 1) || (serial_port > 2))
	  return;
/*	if ((baudrate != 9600) || (baudrate != 19200))
		return;*/


	switch(flag) {
		case 1:	/* set output */
			EE_uart_write_byte(serial_port-1,(EE_UINT8)u[0]);
			break;

		case 2:	/* get input */
			break;
		
		case 4:	/* initialisation */
			EE_uart_init(serial_port-1,baudrate,EE_UART_BIT8_NO|EE_UART_BIT_STOP_1|EE_UART_CTRL_SIMPLE,0);
			break;
		
		case 5:	/* ending */
			EE_uart_close(serial_port-1);
			break;
	}
}
 
