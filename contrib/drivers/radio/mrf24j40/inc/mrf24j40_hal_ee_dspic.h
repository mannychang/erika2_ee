/** 
* @file mrf24j40_hal_ee_dspic.h
* @brief MRF24J40 Hw Abstraction Layer using Erika OS over Microchip dsPIC
* @author Gianluca Franchino
* @author Christian Nastasi
* @author Mauro Nino Marinoni
* @date 2009-03-24
*
* This file is the definition of the hardware abstraction layer used by all the module
* of the MRF24J40 library which uses the Erika kernel drivers. 
*
* \todo Write something about the hal support.
*/

// TODO: Check radio pins.

#ifndef __mrf24j40_hal_ee_dspic_h__
#define __mrf24j40_hal_ee_dspic_h__

#ifndef __USE_SPI__
#error "MRF24J40 HAL EE : The SPI module from MCU is required!"
#endif

#include <mcu/microchip_dspic/inc/ee_spi.h>
#include <cpu/pic30/inc/ee_irqstub.h>

#define MRF24J40_SPI_PORT_1	EE_SPI_PORT_1
#define MRF24J40_SPI_PORT_2	EE_SPI_PORT_2

#ifndef MRF24J40_RESETn
#ifndef __EE_MINIFLEX__
#define MRF24J40_RESETn	PORTGbits.RG0
#else
#define MRF24J40_RESETn	PORTAbits.RA7	
#endif
#endif

#ifndef __EE_MINIFLEX__
	#ifndef MRF24J40_VREG_EN
	#define MRF24J40_VREG_EN	PORTGbits.RG12
	#endif
#endif

#ifndef __EE_MINIFLEX__	
#ifndef MRF24J40_FIFO
#ifdef __USE_DEMOBOARD__	/* Demoboard defaults */
#define MRF24J40_FIFO		PORTDbits.RD14
#else				/* Gianluca's board default*/
#define MRF24J40_FIFO		PORTEbits.RE9
#endif
#endif
#endif

#ifndef MRF24J40_FIFOP
	#ifdef __USE_DEMOBOARD__	/* Demoboard defaults */
	#define MRF24J40_FIFOP		PORTAbits.RA15
	#else 
	#ifdef __EE_MINIFLEX__	/* MiniFlex board default*/
	#define MRF24J40_FIFOP		PORTBbits.RB7
	#else				/* Gianluca's board default*/
	#define MRF24J40_FIFOP		PORTFbits.RF6
	#endif
	#endif
#endif

#ifndef MRF24J40_CSn
#ifndef __EE_MINIFLEX__
#define MRF24J40_CSn		PORTGbits.RG9
#else
#define MRF24J40_CSn		PORTCbits.RC4
#endif
#endif

#ifndef MRF24J40_TRIS_RESETn
#ifdef __EE_MINIFLEX__
#define MRF24J40_TRIS_RESETn	TRISAbits.TRISA7
#else
#define MRF24J40_TRIS_RESETn	TRISGbits.TRISG0

#endif
#endif

#ifndef __EE_MINIFLEX__
#ifndef MRF24J40_TRIS_VREG_EN
#define MRF24J40_TRIS_VREG_EN	TRISGbits.TRISG12
#endif
#endif

#ifndef __EE_MINIFLEX__	
#ifndef MRF24J40_TRIS_FIFO
#ifdef __USE_DEMOBOARD__	/* Demoboard defaults */
#define MRF24J40_TRIS_FIFO	TRISDbits.TRISD14
#else				/* Gianluca's board default*/
#define MRF24J40_TRIS_FIFO	TRISEbits.TRISE9
#endif
#endif
#endif

#ifndef MRF24J40_TRIS_FIFOP
#ifdef __USE_DEMOBOARD__	/* Demoboard defaults */
#define MRF24J40_TRIS_FIFOP	TRISAbits.TRISA15
#else
#ifdef __EE_MINIFLEX__	/* MiniFlex board default*/
#define MRF24J40_TRIS_FIFOP	TRISBbits.TRISB7
#else				/* Gianluca's board default*/
#define MRF24J40_TRIS_FIFOP	TRISFbits.TRISF6
#endif
#endif
#endif

#ifndef MRF24J40_TRIS_CSn
#ifndef __EE_MINIFLEX__
#define MRF24J40_TRIS_CSn	TRISGbits.TRISG9
#else
#define MRF24J40_TRIS_CSn	TRISCbits.TRISC4
#endif	
#endif

#ifdef __USE_DEMOBOARD__	/* Demoboard defaults */

#ifndef MRF24J40_INTERRUPT_NAME	
#define MRF24J40_INTERRUPT_NAME	_INT4Interrupt
#endif

#ifndef MRF24J40_INTERRUPT_FLAG	
#define MRF24J40_INTERRUPT_FLAG	IFS3bits.INT4IF
#endif

#ifndef MRF24J40_INTERRUPT_ENABLE
#define MRF24J40_INTERRUPT_ENABLE IEC3bits.INT4IE
#endif

#ifndef MRF24J40_INTERRUPT_PRIORITY
#define MRF24J40_INTERRUPT_PRIORITY IPC13bits.INT4IP
#endif

#ifndef MRF24J40_INTERRUPT_EDGE_POLARITY	
#define MRF24J40_INTERRUPT_EDGE_POLARITY	 INTCON2bits.INT4EP
#endif

#else /* Gianluca's board and MiniFlex default*/


#ifndef MRF24J40_INTERRUPT_NAME	
#define MRF24J40_INTERRUPT_NAME	_INT0Interrupt
#endif

#ifndef MRF24J40_INTERRUPT_FLAG	
#define MRF24J40_INTERRUPT_FLAG	IFS0bits.INT0IF
#endif

#ifndef MRF24J40_INTERRUPT_ENABLE
#define MRF24J40_INTERRUPT_ENABLE IEC0bits.INT0IE
#endif

#ifndef MRF24J40_INTERRUPT_PRIORITY
#define MRF24J40_INTERRUPT_PRIORITY IPC0bits.INT0IP
#endif

#ifndef MRF24J40_INTERRUPT_EDGE_POLARITY	
#define MRF24J40_INTERRUPT_EDGE_POLARITY	 INTCON2bits.INT0EP
#endif

#endif	/* End default booard selection for ISR */

#define MRF24J40_HAL_ISR() ISR2(MRF24J40_INTERRUPT_NAME)

int8_t	mrf24j40_hal_init(void);
void	mrf24j40_hal_delay_us(uint16_t delay_count); 
int8_t	mrf24j40_hal_spi_init(uint8_t port);
int8_t	mrf24j40_hal_spi_close(void);
int8_t	mrf24j40_hal_spi_write(uint8_t *data, uint16_t len);
int8_t	mrf24j40_hal_spi_read(uint8_t *data, uint16_t len);

COMPILER_INLINE void mrf24j40_hal_retsetn_high(void)
{
	MRF24J40_RESETn = 1;
}

COMPILER_INLINE void mrf24j40_hal_retsetn_low(void)
{
	MRF24J40_RESETn = 0;
}

COMPILER_INLINE void mrf24j40_hal_csn_high(void)
{
	MRF24J40_CSn = 1;
}

COMPILER_INLINE void mrf24j40_hal_csn_low(void)
{
	MRF24J40_CSn = 0;
}

COMPILER_INLINE void mrf24j40_hal_irq_clean(void)
{
	MRF24J40_INTERRUPT_FLAG = 0;
}

COMPILER_INLINE void mrf24j40_hal_irq_enable(void)
{
	MRF24J40_INTERRUPT_ENABLE = 1;
}

COMPILER_INLINE void mrf24j40_hal_irq_disable(void)
{
	MRF24J40_INTERRUPT_ENABLE = 0;
}

COMPILER_INLINE uint8_t mrf24j40_hal_irq_status(void)
{
	return MRF24J40_INTERRUPT_ENABLE;
}


#endif /* Header Protection */
