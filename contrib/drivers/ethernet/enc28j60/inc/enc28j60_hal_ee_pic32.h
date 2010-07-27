/*
  Name: enc28j60_hal_ee_mico32.h
  Copyright: Evidence Srl
  Author: Andrea Azzarà, Marco Ghibaudi
  Date: 29/03/10 18.23
  Description: ENC28J60 driver header file.
*/

#ifndef __ENC28J60_HAL_EE_PIC32_H__
#define __ENC28J60_HAL_EE_PIC32_H__

/* Needed header files */
#include "cpu/pic32/inc/ee_internal.h"
#include "cpu/pic32/inc/ee_irqstub.h"
#include "cpu/pic32/inc/ee_internal.h"
#include "mcu/microchip_pic32/inc/ee_spi.h"

#include "board/ipermob_mb_pic32/inc/ee_ethernet_enc28j60.h"





/* Board header */
#ifdef __IPERMOB_MB_PIC32__

/* 	TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO */
//#include "board/fpga_camera_mico32/inc/ee_internal.h"

/* 	TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO	TODO */

#else	/* NO board */
#error "ENC28J60_HAL: No board selected!"
#endif	



#ifndef __USE_SPI__
#error "ENC28J60 HAL EE : The SPI module from MCU is required!"
#endif
/* Types definition */
#include "enc28j60_reg.h"
#include "enc28j60_compiler.h"

/* ENC28J60 structure (used in ISR mode) */
typedef struct {	
    //MicoGPIO_t* base;					// GPIO controller base address
    //PIC32GPIO_t* base;	
    int irqf;							// GPIO irq flag to register the handler
    EE_TID task;						// task called inside the interrupt handler
} EE_enc28j60_st;

extern EE_enc28j60_st ee_enc28j60_st;

/* ------------------------------------------------------------------------------- */
/* Macros used into the Ethernet driver functions */
/* ------------------------------------------------------------------------------- */

/* Symbols and macros */
#define EE_ENC28J60_DEVICE_ID		(0x01)
#define EE_ENC28J60_ADDRESS_MASK	(0x1F)
#define ENC28J60_INTERRUPT_NAME()	

/* Exit status for the EE_ETHERNET driver */
#define	ENC28J60_SUCCESS 			(0)		/* no error */
#define	ENC28J60_FAILURE			(-19)	/* generic error */
#define	ENC28J60_ERR_SPI_INIT		(-20)	/* error in spi initialization */
#define	ENC28J60_ERR_SPI_READ		(-21)	/* spi read error */
#define	ENC28J60_ERR_SPI_WRITE		(-22)	/* spi write error */
#define	ENC28J60_ERR_DEV_NOINIT		(-23)	/* eth device error */
#define	ENC28J60_ERR_MEM_NULL		(-24)	/* eth null pointer error */

/* Pseudo functions (Wrapper) */
#define ENC_CS_IO_f(p)				/* null definition */
#define ENC_CS_TRIS_f(p)			/* null definition */
#define DelayMs(ms)				EE_enc28j60_delay_ms(ms)
#define Reset()					EE_enc28j60_hardware_reset() // to do... asm("RESET")
#define ClearSPIDoneFlag()			/* null definition */					

/* ------------------------------------------------------------------------------- */
/* ETHERNET driver functions list */
/* ------------------------------------------------------------------------------- */

/* Macros for SPI-based ETHERNET functions */  

__INLINE__ int __ALWAYS_INLINE__ EE_enc28j60_hal_spi_init(void){
	
	//TODO: verificare correttezza della posizione in cui viene richiamata
	// la init del TRIS del CS. Potrebbe essere sensato inizializzarla assieme
	// al RESET.

	EE_ENC28J60_spi_cs_init();
 	EE_spi_init(EE_ENC28J60_SPI_PORT, EE_ENC28J60_SPI_BAUDRATE, 
		EE_ENC28J60_SPI_FLAGS); 
	return ENC28J60_SUCCESS;
}	

__INLINE__ int __ALWAYS_INLINE__  EE_enc28j60_hal_write_byte(EE_UINT8 data){ 
	return EE_spi_write(EE_ENC28J60_SPI_PORT, &data, 1); 
}

__INLINE__ int __ALWAYS_INLINE__  EE_enc28j60_hal_write_buffer(EE_UINT8 *data, int len){ 
	return EE_spi_write(EE_ENC28J60_SPI_PORT, data, len); 
} 

__INLINE__ int __ALWAYS_INLINE__  EE_enc28j60_hal_read_byte(void){ 
	EE_UINT8 *app_read = NULL; 
	if ( EE_spi_read(EE_ENC28J60_SPI_PORT, app_read, 1) != 
			EE_SPI_NO_ERRORS)	
		return ENC28J60_ERR_SPI_READ;
	return *app_read; 
} 


__INLINE__ int __ALWAYS_INLINE__  EE_enc28j60_hal_read_buffer(EE_UINT8 *data, int len){ 
	if ( EE_spi_read(EE_ENC28J60_SPI_PORT, data, len) != 
			EE_SPI_NO_ERRORS)	
		return ENC28J60_ERR_SPI_READ;
	return ENC28J60_SUCCESS;   

} 


/* This functions is used to select the chip select */
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_set_SSO(void){ 
	EE_ENC28J60_set_CS();
}

/* This functions is used to deselect the the chip select */
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_clear_SSO(void){ 
	EE_ENC28J60_clear_CS(); 
}


void EE_enc28j60_hal_handler(int level);


/*TODO: non dovrebbe essere mai chiamata per cui è stata tolta */
//#define EE_enc28j60_hal_handler_setup()




#define EE_ENC28J60_clear_irq_flag()	(EE_ENC28J60_IRQ_SPECIFIC_FLAG = 0)







#define EE_enc28j60_hal_chip_select() 	EE_enc28j60_hal_set_SSO()
#define EE_enc28j60_hal_chip_unselect() EE_enc28j60_hal_clear_SSO()

/**
	This function sets the task should be called inside the interrupt handler.
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_set_rx_task(EE_TID task)
{
	ee_enc28j60_st.task = task;
}

/**
	This function writes 2 bytes on spi bus. 
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_write_16(EE_UINT8 byte1, EE_UINT8 byte2)
{
	EE_enc28j60_hal_write_byte(byte1);
	EE_enc28j60_hal_write_byte(byte2);
}

/**
	This function contains a delay loop. 
*/
void EE_enc28j60_hal_delay_us(unsigned int delay_count);

/**
	This function contains a delay loop. 
*/
void EE_enc28j60_hal_delay_ms(unsigned int delay_count);

/**
	This function reads an ETH register.
*/
REG EE_enc28j60_hal_read_ETH_register(BYTE Address);

/**
	This function reads a MAC/MII register.
*/
REG EE_enc28j60_hal_read_MAC_MII_register(BYTE Address);

/**
	This function reads a PHY register.
*/
PHYREG EE_enc28j60_hal_read_PHY_register(BYTE Register);

/**
	This function writes on a generic register.
*/
void EE_enc28j60_hal_write_register(BYTE Address, BYTE Data);

/**
	This function writes on a PHY register.
*/
void EE_enc28j60_hal_write_PHY_register(BYTE Register, WORD Data);

/**
	These functions read from the ENC28J60 memory buffer.
*/
BYTE EE_enc28j60_hal_get();
WORD EE_enc28j60_hal_get_array(BYTE *val, WORD len);

/**
	These functions write on the ENC28J60 memory buffer.
*/
void EE_enc28j60_hal_put(BYTE val);
void EE_enc28j60_hal_put_array(BYTE *val, WORD len);

/**
	This function sets up to 8 bits in a ETH register.
*/
void EE_enc28j60_hal_bit_field_set_register(BYTE Address, BYTE Data);

/** 
	This function clears up to 8 bits in a ETH register.
*/
void EE_enc28j60_hal_bit_field_clear_register(BYTE Address, BYTE Data);

/**
	This function send a software reset command.
*/
void EE_enc28j60_hal_software_reset(void);

/**
	This function resets the device (HW reset).
*/
void EE_enc28j60_hal_hardware_reset(void);

/**
	This function selects the bank.
*/
void EE_enc28j60_hal_bank_select(WORD Register);

/* ---------------- */
/* INLINE functions */
/* ---------------- */

/**
	This function reads a MAC register.
*/
__INLINE__ REG __ALWAYS_INLINE__ EE_enc28j60_hal_read_MAC_register(BYTE address)
{
	return EE_enc28j60_hal_read_MAC_MII_register(address);
}

/**
	This function reads a MII register.
*/
__INLINE__ REG __ALWAYS_INLINE__ EE_enc28j60_hal_read_MII_register(BYTE address)
{
	return EE_enc28j60_hal_read_MAC_MII_register(address);
}

/**
	This function writes on a ETH register.
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_write_ETH_register(BYTE address, BYTE data)
{
	return EE_enc28j60_hal_write_register(address, data);
}

/**
	This function writes on a MAC register.
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_write_MAC_register(BYTE address, BYTE data)
{
	return EE_enc28j60_hal_write_register(address, data);
}

/**
	This function writes on a MII register.
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_write_MII_register(BYTE address, BYTE data)
{
	return EE_enc28j60_hal_write_register(address, data);
}

/**
	This function enables device using reset pin (turn on). 
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_enable(void)
{
		
	EE_enc28j60_gpio_release_reset();
}

/**
	This function disables device using reset pin (turn off). 
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_disable(void)
{
	EE_enc28j60_gpio_hold_in_reset();
}

/**
	This function enables ENC28J60 reception interrupts. 
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_enable_IRQ(void)
{
	//mico32_enable_irq(ee_enc28j60_st.irqf);
	EE_enc28j60_gpio_enable_IRQ();
}

/**
	This function disables ENC28J60 reception interrupts. 
*/
__INLINE__ void __ALWAYS_INLINE__ EE_enc28j60_hal_disable_IRQ(void)
{
	EE_enc28j60_gpio_disable_IRQ();
}

/**
	This function returns ENC28J60 reception interrupts enabling status. 
*/
__INLINE__ int __ALWAYS_INLINE__ EE_enc28j60_hal_IRQ_enabled(void)
{
	return EE_enc28j60_gpio_IRQ_enabled();
}

#endif //__ENC28J60_HAL_EE_PIC32_H__

