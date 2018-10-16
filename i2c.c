#include <msp430.h>
#include <stdint.h>

#include "globals.h"

uint8_t 	i2c_cntr;
uint8_t 	i2c_reg_addr;
uint8_t 	i2c_rx_buf;
uint8_t 	i2c_rx_MSB, i2c_rx_LSB;
uint16_t	i2c_rx_16bit;
uint16_t 	i2c_reg[64];
uint8_t		unit_mode;


void write_register(uint8_t i2c_reg_addr, uint16_t val) {
	if(i2c_reg_addr + unit_mode < 64) {
		for(;;);
	}; //return;
	if(i2c_reg_addr + unit_mode > 127) {
		for(;;);
	}; //return;
	if(val > 1023) {
		for(;;);
	};//return;
	i2c_reg[i2c_reg_addr - SYSEX_RANGE_CHAIN_START + unit_mode] = val;
}

uint16_t read_register(uint8_t i2c_reg_addr) {
	if((i2c_reg_addr + unit_mode) < 64) return(0);
	if((i2c_reg_addr + unit_mode) > 127) return(0);
	if(i2c_reg[i2c_reg_addr - SYSEX_RANGE_CHAIN_START + unit_mode] > 1023) return(0);
	return(i2c_reg[i2c_reg_addr - SYSEX_RANGE_CHAIN_START + unit_mode]);
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR_HOOK(void) {
	//
	// TX interrupt is called for RX __and__ TX
	//
	if (IFG2 & UCB0TXIFG) {									// this is a READ CALL, send response
		switch (i2c_reg_addr) {								// callback register
		case REG_B_DEVTYPE:								// identify itself
			UCB0TXBUF = DEV_TYPE_FILTER_GAIN;					// send TX reply
			break;
		default:									// if not known register, default reply
			UCB0TXBUF = 0xff;							// send TX reply
			break;
		}
	} else if (IFG2 & UCB0RXIFG) {								// this is a WRITE CALL
		i2c_rx_buf = 0;									// always zero 16bit register
		i2c_rx_buf = UCB0RXBUF;								// load rx_buf is 16bit, but data is only 8bit
		if (i2c_cntr == 0) {								// if i2c_cntr is 0 this byte contains register address
			i2c_reg_addr = i2c_rx_buf;						// save register address in i2c_reg_addr
			i2c_cntr++;								// increment packet counter
		} else {
			switch (i2c_cntr) {							// packet counter in not 0, we do process a message
			case 1:									//
				i2c_rx_LSB = i2c_rx_buf;					// !!! Adafruit I2C lib sends LSB first !!!
				i2c_cntr++;							// increment packet counter
				break;
			case 2:									// do 16bit register at once in IRQ
				i2c_rx_MSB = i2c_rx_buf;					// !!! Adafruit I2C lib sends LSB first !!!
				i2c_rx_16bit = i2c_rx_MSB;
				i2c_rx_16bit = i2c_rx_16bit << 8;
				i2c_rx_16bit |= i2c_rx_LSB;
				if(i2c_reg_addr > 63) {
					if(i2c_reg_addr < 128) {
						i2c_reg[i2c_reg_addr - SYSEX_RANGE_CHAIN_START] = i2c_rx_16bit;
					}
				}								// write with a function (input validation)
				i2c_cntr++;							// increment packet counter
				break;
			}
		}
	}
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR_HOOK(void) {
	//
	// RX interrupt is called for STATE handling ONLY!
	//
	if (UCB0STAT & UCSTTIFG) {								// START condition interrupt (1 is pending)
		i2c_cntr = 0;									// transmission begins
		UCB0STAT &= ~UCSTTIFG;                 						// clear start condition int flag
	} else if (UCB0STAT & UCSTPIFG) {							// STOP condition interrupt (1 is pending)
		UCB0STAT &= ~UCSTPIFG;                  					// clear stop condition int flag
	} else if (UCB0STAT & UCNACKIFG) {							// NACK - slave not acknowledge received data (if master)
		UCB0STAT &= ~UCNACKIFG;
	} else if (UCB0STAT & UCALIFG) {							// two or more masters start trans. simultaneously
												// master code
	}
}

void set_device_type(uint8_t i2c_dev_address) {
	if((i2c_dev_address & 0x04) == 0x04) {
		unit_mode = UNIT_MODE_WAVESHAPER;
	} else {
		unit_mode = UNIT_MODE_FILTER;
	};
};

uint8_t set_i2c_addr(void) {									// set i2c address
	uint8_t i2c_dev_address;
	i2c_dev_address = P1IN;
	i2c_dev_address = (~i2c_dev_address) & 0x3f;
	set_device_type(i2c_dev_address);
	i2c_dev_address += I2C_BASE_ADDR;
	return(i2c_dev_address);
};

void I2c_Init(void) {										// init i2c
	// i2c
	UCB0CTL1 |= UCSWRST; 									/* Disable USCI */
	UCB0CTL0 = UCMODE_3 | UCSYNC; 								/* UCMODE_3 -- I2C Mode, UCSYNC -- Synchronous Mode */
	UCB0I2COA = set_i2c_addr(); 								/* I2C Own Address Register */
	UCB0BR0 = 160; 										/* Bit Rate Control Register 0 */
	UCB0I2CIE = UCSTTIE; 									/* UCSTTIE -- Interrupt enabled */
	UCB0CTL1 &= ~UCSWRST; 									/* Enable USCI */
	IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);							/* IFG2, Interrupt Flag Register 2 */
	IE2 |= UCB0TXIE | UCB0RXIE;								/* IE2, Interrupt Enable Register 2 */
}
