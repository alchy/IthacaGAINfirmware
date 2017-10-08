/*
 * setup.c
 *
 *  Created on: 1. 3. 2016
 *      Author: Jindra
 */

#include <msp430.h>
#include <stdint.h>

#include "globals.h"
#include "i2c.h"
#include "conf_gpio.h"

void BCSplus_Init(void) {
	BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;

	if (CALBC1_16MHZ != 0xFF) {
		__delay_cycles(100000); 							/* Adjust this accordingly to your VCC rise time */
		DCOCTL = 0x00;
		BCSCTL1 = CALBC1_16MHZ; 							/* Set DCO to 16MHz */
		DCOCTL = CALDCO_16MHZ;
	}

	BCSCTL1 |= XT2OFF | DIVA_0;
	BCSCTL3 = XT2S_0 | LFXT1S_0 | XCAP_1;
}

void Timer0_A3_Init(void) {
	TA0CCTL0 = CM_0 | CCIS_0 | OUTMOD_4 | CCIE;
	TA0CCTL1 = CM_0 | CCIS_0 | OUTMOD_3;
	TA0CCTL2 = CM_0 | CCIS_0 | OUTMOD_3;
	TA0CCR0 = PWMBASE;
	TA0CCR1 = PWMBASE / 2;
	TA0CCR2 = PWMBASE / 2;
	TA0CTL = TASSEL_2 | ID_0 | MC_1;
}

void Timer1_A3_Init(void) {
	TA1CCTL0 = CM_0 | CCIS_0 | OUTMOD_4 | CCIE;
	TA1CCTL1 = CM_0 | CCIS_0 | OUTMOD_3;
	TA1CCTL2 = CM_0 | CCIS_0 | OUTMOD_3;
	TA1CCR0 = PWMBASE;
	TA1CCR1 = PWMBASE / 2;
	TA1CCR2 = PWMBASE / 2;
	TA1CTL = TASSEL_2 | ID_0 | MC_1;
}

void GPIO_Init(void) {
	// i2c
	P1SEL2 = BIT6 | BIT7;									/* Port 1 Port Select 2 Register */
	P1SEL = BIT6 | BIT7;									/* Port 1 Port Select Register */
	P1OUT = 0;												/* Port 1 Output Register */
	P1IES = 0;												/* Port 1 Interrupt Edge Select Register */
	P1IFG = 0;												/* Port 1 Interrupt Flag Register */

	// i2c addr reader pin with pull-up
	conf_pin_as_input(PORT1, PIN0, R_PULLUP, IRQ_NO, EDGE_NONE);
	conf_pin_as_input(PORT1, PIN1, R_PULLUP, IRQ_NO, EDGE_NONE);
	conf_pin_as_input(PORT1, PIN2, R_PULLUP, IRQ_NO, EDGE_NONE);
	conf_pin_as_input(PORT1, PIN3, R_PULLUP, IRQ_NO, EDGE_NONE);
	conf_pin_as_input(PORT1, PIN4, R_PULLUP, IRQ_NO, EDGE_NONE);
	conf_pin_as_input(PORT1, PIN5, R_PULLUP, IRQ_NO, EDGE_NONE);

	// timer1
	P2OUT = 0; 												/* Port 2 Output Register */
	P2SEL = BIT2 | BIT4; 									/* Port 2 Port Select Register */
	P2SEL &= ~(BIT6 | BIT7); 								/* Port 2 Port Select Register */
	P2DIR = BIT2 | BIT4; 									/* Port 2 Direction Register */
	P2IES = 0; 												/* Port 2 Interrupt Edge Select Register */
	P2IFG = 0; 												/* Port 2 Interrupt Flag Register */

	// timer2
	P3OUT = 0; 												/* Port 3 Output Register */
	P3SEL = BIT5 | BIT6; 									/* Port 3 Port Select Register */
	P3DIR = BIT5 | BIT6; 									/* Port 3 Direction Register */
}


void setup(void) {

	// watchdog configuration
	WDTCTL = WDTPW | WDTHOLD;							// stop watchdog timer
	WDTCTL = WDT_MDLY_0_5;								// WD irq each 8000 cycles (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS0)
														// WDT_MDLY_32 	   32000 cycles
														// WDT_MDLY_8		8000 cycles (500us at 16MHz)
														// WDT_MDLY_0_5		 500 cycles ( 31us at 16MHz)
														// WDT_MDLY_0_064     64 cycles (  4us at 16MHz)
	IE1 |= WDTIE;										// Enable WDT interrupt

	// other modules
	BCSplus_Init();										// init clocks
	GPIO_Init();										// init gpio
	Timer0_A3_Init();									// init timer0
	Timer1_A3_Init();									// init timer1
	I2c_Init();											// I2c init - separate include i2c.c/h

	__bis_SR_register(GIE);								// general interupt enable

}
