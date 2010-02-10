/** 
* @file mrf24j40_hal_ee_pic32.c
* @brief MRF24J40 Hw Abstraction Layer using Erika OS over microchip pic32.
* @author Christian Nastasi
* @author Marco Ghibaudi 
* @date 2010-01-12
*
* This file is the  hardware abstraction layer used by all the module
* of the MRF24J40 library which uses the Erika kernel drivers. 
*
* \todo Write something about the hal support.
*/
#include <mrf24j40_hal.h>

#ifdef __MICROCHIP_PIC32__

static uint8_t spi_port; 

void mrf24j40_delay_us(uint16_t delay_count) 
{	/* Provide a delay with an accuracy of approx. 1.6% @ 80Mhz*/
	asm volatile(	"add $t0, $a0, $zero\n\t"			  	
			"1:\n\t"
			"addi $t0, $t0, -1\n\t"
			"nop\n\t"
			"nop\n\t"
			"nop\n\t"
			"bne $t0, $zero, 1b\n\t");			
}

int8_t	mrf24j40_hal_init(void)
{
	/* Set the IO pins direction */
	MRF24J40_TRIS_RESETn = 0;
	MRF24J40_TRIS_INT = 1;
	MRF24J40_TRIS_CSn = 0;
	MRF24J40_TRIS_VREG_EN = 0;
	/* Set interrupt registers */
	MRF24J40_INTERRUPT_FLAG = 0;
	MRF24J40_INTERRUPT_PRIORITY = 1;
	MRF24J40_INTERRUPT_EDGE_POLARITY = 0;
	MRF24J40_INTERRUPT_ENABLE = 1;
	return 1;
}

int8_t	mrf24j40_spi_init(uint8_t port)
{
	spi_port = port;
	return EE_spi_init(spi_port, 10000000, EE_SPI_MASTER | 
			    EE_SPI_SDO_ON_CLOCK_TO_IDLE);
}

int8_t	mrf24j40_spi_close(void)
{
	return EE_spi_close(spi_port);
}

int8_t	mrf24j40_spi_write(uint8_t *data, uint16_t len)
{
	return EE_spi_write(spi_port, data, len);
}

int8_t	mrf24j40_spi_read(uint8_t *data, uint16_t len)
{
	return EE_spi_read(spi_port, data, len);
}

#else

#error "MRF24J40_HAL: Architecture not supported!"

#endif