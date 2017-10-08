#include "conf_gpio.h"

#include <msp430.h> 

/*
 *  The GPIO registers should be set in a specific order:
 *     PxOUT
 *     PxSEL or PxSELx
 *     PxDIR
 *     PxREN
 *     PxIES
 *     PxIFG
 *     PxIE
 */


void conf_clock_16MHz(void) {

	__delay_cycles(100000);								/* Adjust this accordingly to your VCC rise time */

    if (CALBC1_16MHZ != 0xFF) {
        DCOCTL = 0x00;
        DCOCTL = CALDCO_16MHZ;
        BCSCTL1 = CALBC1_16MHZ;     					/* Set DCO to 16MHz */
        BCSCTL2 &= ~(DIVS_0);							/* SMCLK = DCO / DIVS = nMHz */
        BCSCTL3 |= LFXT1S_2;							/* ACLK = VLO = ~ 12 KHz */

    } else {
    	for(;;);
    }

    /*
     * Basic Clock System Control 1
     *
     * XT2OFF -- Disable XT2CLK
     * ~XTS -- Low Frequency
     * DIVA_0 -- Divide by 1
     *
     * Note: ~XTS indicates that XTS has value zero
     */
    BCSCTL1 |= XT2OFF | DIVA_0;
}


void conf_calibrate(void) {
	conf_pin_function(PORT2, PIN6, 0, 1, 0);
	conf_pin_function(PORT2, PIN7, 1, 1, 0);;
	// now the code can run crystal calibrate routine
}


void conf_pin_function(	unsigned char port,
						unsigned char pin,
						unsigned char pxdir_val,
						unsigned char pxsel_val,
						unsigned char pxsel2_val) {

	volatile unsigned char *pxsel2, *pxsel, *pxdir;

	switch(port) {
		case 1:
			pxdir = (volatile unsigned char *) _P1DIR_;
			break;

		case 2:
			pxdir = (volatile unsigned char *) _P2DIR_;
			break;

		case 3:
			pxdir = (volatile unsigned char *) _P3DIR_;
			break;
	}

	switch(pxdir_val) {

		case 0:
			*pxdir  &=  ~(1<<pin);

		case 1:
			*pxdir  |=  (1<<pin);
			break;
	}

	switch(port) {
			case 1:
				pxsel2 = (volatile unsigned char *) _P1SEL2_;
				pxsel = (volatile unsigned char *) _P1SEL_;
				break;

			case 2:
				pxsel2 = (volatile unsigned char *) _P2SEL2_;
				pxsel = (volatile unsigned char *) _P2SEL_;
				break;

			case 3:
				pxsel2 = (volatile unsigned char *) _P3SEL2_;
				pxsel = (volatile unsigned char *) _P3SEL_;
				break;
		}

	switch(pxsel2_val) {
		case 0:
			*pxsel2 &= (1<<pin);                  			// I/O function is selected.
			break;

		case 1:
			*pxsel2 |= (1<<pin);								// Primary peripheral module function is selected.
			break;
	}

	switch(pxsel_val) {
		case 0:
			*pxsel &= (1<<pin);							// I/O function is selected.
			break;

		case 1:
			*pxsel |= (1<<pin);							// Primary peripheral module function is selected.
			break;
	}

}

void conf_pin_as_output(unsigned char port, unsigned char pin) {

	volatile unsigned char *pxout, *pxsel2, *pxsel, *pxdir;

	// configure as GPIO
	switch(port) {
		case 1:
			pxsel2 = (volatile unsigned char *) _P1SEL2_;
			pxsel  = (volatile unsigned char *) _P1SEL_;
			pxout  = (volatile unsigned char *) _P1OUT_;
			break;

		case 2:
			pxsel2 = (volatile unsigned char *) _P2SEL2_;
			pxsel  = (volatile unsigned char *) _P2SEL_;
			pxout  = (volatile unsigned char *) _P2OUT_;
			break;

		case 3:
			pxsel2 = (volatile unsigned char *) _P3SEL2_;
			pxsel  = (volatile unsigned char *) _P3SEL_;
			pxout  = (volatile unsigned char *) _P2OUT_;
			break;
	}

	// configure as OUTPUT
	switch(port) {
		case 1:
			pxdir = (volatile unsigned char *) _P1DIR_;
			break;

		case 2:
			pxdir = (volatile unsigned char *) _P2DIR_;
			break;

		case 3:
			pxdir = (volatile unsigned char *) _P3DIR_;
			break;
	}
	*pxout  &= ~(1<<pin);									// out is LOW
    *pxsel2 &= ~(1<<pin);                  					// I/O function is selected.
	*pxsel  &= ~(1<<pin);									// I/O function is selected.
	*pxdir  |=  (1<<pin);									// Bit = 1: The port pin is switched to output direction.
}


void conf_pin_output(unsigned char port, unsigned char pin, unsigned char state) {

	switch(port) {
		case 1:
			if(state == 0) {
				P1OUT &= ~(1<<pin);
			} else if(state == 1) {
				P1OUT |=  (1<<pin);
			} else {
				P1OUT ^=  (1<<pin);
			}
			break;

		case 2:
			if(state == 0) {
				P2OUT &= ~(1<<pin);
			} else if(state == 1) {
				P2OUT |=  (1<<pin);
			} else {
				P2OUT ^=  (1<<pin);
			}
			break;

		case 3:
			if(state == 0) {
				P3OUT &= ~(1<<pin);
			} else if(state == 1) {
				P3OUT |=  (1<<pin);
			} else {
				P3OUT ^=  (1<<pin);
			}
			break;
	}
}

void conf_pin_as_input(unsigned char port, \
					   unsigned char pin, \
					   unsigned char resistor, \
					   unsigned char irq, \
					   unsigned char edge_select) {

	volatile unsigned char *pxsel2, *pxsel, *pxout, *pxdir, *pxren;

	// configure as GPIO
	switch(port) {
		case 1:
			pxsel2 = (volatile unsigned char *) _P1SEL2_;
			pxsel = (volatile unsigned char *) _P1SEL_;
			break;

		case 2:
			pxsel2 = (volatile unsigned char *) _P2SEL2_;
			pxsel = (volatile unsigned char *) _P2SEL_;
			break;

		case 3:
			pxsel2 = (volatile unsigned char *) _P3SEL2_;
			pxsel = (volatile unsigned char *) _P3SEL_;
			break;
	}

	// configure as INPUT
	switch(port) {
		case 1:
			pxdir = (volatile unsigned char *) _P1DIR_;
			pxren = (volatile unsigned char *) _P1REN_;
			pxout = (volatile unsigned char *) _P1OUT_;
			break;

		case 2:
			pxdir = (volatile unsigned char *) _P2DIR_;
			pxren = (volatile unsigned char *) _P2REN_;
			pxout = (volatile unsigned char *) _P2OUT_;
			break;

		case 3:
			pxdir = (volatile unsigned char *) _P3DIR_;
			pxren = (volatile unsigned char *) _P3REN_;
			pxout = (volatile unsigned char *) _P3OUT_;
			break;
	}

	switch(resistor) {
		case R_DISABLED:
			*pxout  &= ~(1<<pin);							// does not apply
			*pxsel2 &= ~(1<<pin);                 			// I/O function is selected.
			*pxsel  &= ~(1<<pin);							// I/O function is selected.
			*pxdir  &= ~(1<<pin);      						// Bit = 0: The port pin is switched to input direction.
			*pxren  &= ~(1<<pin);            				// Bit = 0: Pullup/pulldown resistor disabled.
			break;

		case R_PULLUP:
			*pxout  |= (1<<pin);							// pullup
			*pxsel2 &= ~(1<<pin);                 			// I/O function is selected.
			*pxsel  &= ~(1<<pin);							// I/O function is selected.
			*pxdir  &= ~(1<<pin);      						// Bit = 0: The port pin is switched to input direction.
			*pxren  |= (1<<pin);							// Bit = 1: Pullup/pulldown resistor enabled
			break;

		case R_PULLDOWN:
			*pxout  &= ~(1<<pin);							// pulldown (to avoid spurious interrupts)
			*pxsel2 &= ~(1<<pin);                			// I/O function is selected.
			*pxsel  &= ~(1<<pin);							// I/O function is selected.
			*pxdir  &= ~(1<<pin);      						// Bit = 0: The port pin is switched to input direction.
			*pxren  |= (1<<pin);							// Bit = 1: Pullup/pulldown resistor enabled
			break;
	}

	if(irq == IRQ_YES) {
		conf_pin_irq_enable(port, pin, edge_select);
	} else {
		conf_pin_irq_disable(port, pin);
	}
}


void conf_pin_irq_enable(unsigned char port, unsigned char pin, unsigned char edge_select) {

	volatile unsigned char *pxie, *pxies, *pxifg;

	switch(port) {
		case 1:
			pxie = (volatile unsigned char *) _P1IE_;
			pxies = (volatile unsigned char *) _P1IES_;
			pxifg = (volatile unsigned char *) _P1IFG_;
			break;

		case 2:
			pxie = (volatile unsigned char *) _P2IE_;
			pxies = (volatile unsigned char *) _P2IES_;
			pxifg = (volatile unsigned char *) _P2IFG_;
			break;
	}

	switch(edge_select) {
		case EDGE_HIGH2LOW:
			*pxies |= (1<<pin); 										// Bit = 1: The PxIFGx flag is set with a high-to-low transition.
			break;

		case EDGE_LOW2HIGH:												// Bit = 0: The PxIFGx flag is set with a low-to-high transition.
			*pxies &= ~(1<<pin);
			break;
	}

	*pxie |= (1<<pin);                          						// Bit = 1: The interrupt is enabled.
	*pxifg &= ~(1<<pin);												// clear interrupt for this pin
}

void conf_pin_irq_disable(unsigned char port, unsigned char pin) {

	volatile unsigned char *pxie;

	switch(port) {
		case 1:
			pxie = (volatile unsigned char *) _P1IE_;
			break;

		case 2:
			pxie = (volatile unsigned char *) _P2IE_;
			break;
	}

	*pxie &= ~(1<<pin);                         					// Bit = 0: The interrupt is disabled.
}

unsigned char get_pin_irq_flags_and_clear(unsigned char port) {

	unsigned char pxifg_flags;

	switch(port) {
		case 1:
			pxifg_flags = P1IFG;
			P1IFG &= 0;                               			// P0.all IFG cleared
			return pxifg_flags;

		case 2:
			pxifg_flags = P2IFG;
			P2IFG &= 0;											// P1.all IFG cleared
			return pxifg_flags;
	}
	return 0;
}
