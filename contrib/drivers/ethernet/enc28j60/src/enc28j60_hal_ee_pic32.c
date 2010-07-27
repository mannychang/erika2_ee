/*
  Name: enc28j60_hal_ee_mico32.c
  Copyright: Evidence Srl
  Author: Dario Di Stefano
  Date: 29/03/10 18.23
  Description: ENC28J60 driver source file.
*/

#include "enc28j60.h"
#include "cpu/pic32/inc/ee_utils.h"


/* ---------------------- Global variables --------------------------------- */
EE_enc28j60_st ee_enc28j60_st = {
	//.base= (MicoGPIO_t* )EE_ENC28J60_BASE_ADDRESS,
	//.irqf= EE_ENC28J60_IRQ, 
	.task= -1 };
	
/* ---------------------- Ethernet interrupt handler ------------------------- */

/**
 * The PKTIF bit can only be cleared by the host controller
 * or by a Reset condition. In order to clear PKTIF, the
 * EPKTCNT register must be decremented to 0. If the last
 * data packet in the receive buffer is processed,
 * EPKTCNT will become zero and the PKTIF bit will
 * automatically be cleared.
*/


void EE_enc28j60_hal_handler(int level)
{
	EE_enc28j60_disable_IRQ();

	/* Clear GPIO irq pending flag */

	EE_ENC28J60_clear_irq_flag();	
	//EE_gpio_common_handler(level);

	/* Called task should re-enable IRQs */
	if( ee_enc28j60_st.task >= 0)
		 ActivateTask(ee_enc28j60_st.task);	
		 
	return;
}

/* ---------------------- Ethernet Library functions ------------------------- */


void EE_enc28j60_hal_delay_us(unsigned int delay_count)
{
	EE_delay_us(delay_count);
	//MicoSleepMicroSecs((unsigned int)delay_count);
}

void EE_enc28j60_hal_delay_ms(unsigned int delay_count)
{
	EE_delay_us((delay_count*1000));
	//MicoSleepMilliSecs((unsigned int)delay_count);
}


BYTE EE_enc28j60_hal_get()
{
	BYTE Result;	
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_byte(ENC28J60_RBM);
	Result = EE_enc28j60_hal_read_byte();
	EE_enc28j60_hal_chip_unselect();
	return Result;
}//end MACGet


WORD EE_enc28j60_hal_get_array(BYTE *val, WORD len)
{
	WORD num;
	EE_pic32_disableIRQ();
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_byte(ENC28J60_RBM);
	EE_enc28j60_write_timestamp(ENC28J60_START_GET_ARRAY);
	num = EE_enc28j60_hal_read_buffer(val, len);
	EE_enc28j60_write_timestamp(ENC28J60_END_GET_ARRAY);	//TODO: verificare se si può utilizzare sul PIC32
	EE_enc28j60_hal_chip_unselect();
    return num;
}//end MACGetArray




void EE_enc28j60_hal_put(BYTE val)
{	
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_16(ENC28J60_WBM, val);
	EE_enc28j60_hal_chip_unselect();
}//end MACPut

//BIBO: ok
void EE_enc28j60_hal_put_array(BYTE *val, WORD len)
{	
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_byte(ENC28J60_WBM);
	EE_enc28j60_write_timestamp(ENC28J60_START_PUT_ARRAY);
	EE_enc28j60_hal_write_buffer(val, len);
	EE_enc28j60_write_timestamp(ENC28J60_END_PUT_ARRAY);
	EE_enc28j60_hal_chip_unselect();
}//end MACPutArray

void EE_enc28j60_hal_software_reset(void)
{
	// Note: The power save feature may prevent the reset from executing, so
    // we must make sure that the device is not in power save before issuing
    // a reset.
	EE_enc28j60_hal_bit_field_clear_register(ECON2, ECON2_PWRSV);
    // Give some opportunity for the regulator to reach normal regulation and
    // have all clocks running
    EE_enc28j60_hal_delay_us(1000);
	// Execute the System Reset command
	EE_enc28j60_hal_write_byte(ENC28J60_SR);
    // Wait for the oscillator start up timer and PHY to become ready
    EE_enc28j60_hal_delay_us(1000);
}//end SendSystemReset

void EE_enc28j60_hal_hardware_reset(void)
{
	EE_enc28j60_hal_disable();
	EE_enc28j60_hal_delay_us(1000);
	EE_enc28j60_hal_enable();
}

REG EE_enc28j60_hal_read_ETH_register(BYTE Address)
{
    REG r;
		
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_byte(ENC28J60_RCR | Address);
	r.Val = EE_enc28j60_hal_read_byte();
	EE_enc28j60_hal_chip_unselect();
	
    return r;
}//end ReadETHReg

REG EE_enc28j60_hal_read_MAC_MII_register(BYTE Address)
{
    REG r;
		
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_byte(ENC28J60_RCR | Address);
	EE_enc28j60_hal_read_byte();
	r.Val = EE_enc28j60_hal_read_byte();
	EE_enc28j60_hal_chip_unselect();
	
    return r;
}//end ReadMACReg

PHYREG EE_enc28j60_hal_read_PHY_register(BYTE Register)
{
    PHYREG Result;

    // Set the right address and start the register read operation
    BankSel(MIREGADR);
    WriteReg((BYTE)MIREGADR, Register);
    WriteReg((BYTE)MICMD, MICMD_MIIRD);

    // Loop to wait until the PHY register has been read through the MII
    // This requires 10.24us
    BankSel(MISTAT);
	while(ReadMACReg((BYTE)MISTAT).Val & MISTAT_BUSY);
	
	// Stop reading
    BankSel(MIREGADR);
    WriteReg((BYTE)MICMD, 0x00);
	Result.VAL.byte.LB = ReadMACReg((BYTE)MIRDL).Val;
    Result.VAL.byte.HB = ReadMACReg((BYTE)MIRDH).Val;
    BankSel(ERDPTL);    // Return to Bank 0
	
    return Result;
}//end ReadPHYReg

void EE_enc28j60_hal_write_register(BYTE Address, BYTE Data)
{
	
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_16(ENC28J60_WCR | Address, Data);
	EE_enc28j60_hal_chip_unselect();
}//end WriteReg

void EE_enc28j60_hal_bit_field_clear_register(BYTE Address, BYTE Data)
{
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_16(ENC28J60_BFC | Address, Data);
	EE_enc28j60_hal_chip_unselect();
}//end BFCReg

void EE_enc28j60_hal_bit_field_set_register(BYTE Address, BYTE Data)
{
	EE_enc28j60_hal_chip_select();
	EE_enc28j60_hal_write_16(ENC28J60_BFS | Address, Data);
	EE_enc28j60_hal_chip_unselect();
}//end BFSReg

void EE_enc28j60_hal_write_PHY_register(BYTE Register, WORD Data)
{
    // Write the register address
    BankSel(MIREGADR);
    WriteReg((BYTE)MIREGADR, Register);

	// Write the data
    // Order is important: write low byte first, high byte last
    WriteReg((BYTE)MIWRL, ((WORD_VAL*)&Data)->byte.LB);
    WriteReg((BYTE)MIWRH, ((WORD_VAL*)&Data)->byte.HB);
	
    // Wait until the PHY register has been written
    BankSel(MISTAT);
	
	while(ReadMACReg((BYTE)MISTAT).Val & MISTAT_BUSY);

    BankSel(ERDPTL);    // Return to Bank 0
}//end WritePHYReg

void EE_enc28j60_hal_bank_select(WORD Register)
{
	BFCReg(ECON1, ECON1_BSEL1 | ECON1_BSEL0);
    BFSReg(ECON1, ((WORD_VAL*)&Register)->byte.HB);
	
}//end BankSel

void EE_enc28j60_hal_set_clkout(BYTE NewConfig)
{
    BankSel(ECOCON);
    WriteReg((BYTE)ECOCON, NewConfig);
    BankSel(ERDPTL);
}//end SetCLKOUT

BYTE EE_enc28j60_hal_get_clkout(void)
{
    BYTE i;

    BankSel(ECOCON);
    i = ReadETHReg((BYTE)ECOCON).Val;
    BankSel(ERDPTL);
    return i;
}//end GetCLKOUT



