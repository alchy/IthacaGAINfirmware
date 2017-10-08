/*
 * gpio_config.h
 *
 *  Created on: 18. 5. 2015
 *      Author: Jindra
 */

/************************************************************
* DIGITAL I/O Port1/2 Pull up / Pull down Resistors
************************************************************/
#define				_P1IN_               0x0020
#define				_P1OUT_              0x0021
#define				_P1DIR_              0x0022
#define				_P1IFG_              0x0023
#define				_P1IES_              0x0024
#define				_P1IE_               0x0025
#define				_P1SEL_              0x0026
#define				_P1SEL2_			 0x0041
#define				_P1REN_              0x0027
#define				_P2IN_               0x0028
#define				_P2OUT_              0x0029
#define				_P2DIR_              0x002A
#define				_P2IFG_              0x002B
#define				_P2IES_              0x002C
#define				_P2IE_               0x002D
#define				_P2SEL_              0x002E
#define				_P2SEL2_             0x0042
#define				_P2REN_              0x002F
/************************************************************
* DIGITAL I/O Port3/4 Pull up / Pull down Resistors
************************************************************/
#define				_P3IN_               0x0018
#define				_P3OUT_              0x0019
#define				_P3DIR_              0x001A
#define				_P3SEL_              0x001B
#define				_P3SEL2_             0x0043
#define				_P3REN_              0x0010
#define				_P4IN_               0x001C
#define				_P4OUT_              0x001D
#define				_P4DIR_              0x001E
#define				_P4SEL_              0x001F
#define				_P4REN_              0x0011


#ifndef GPIO_CONFIG_H_
#define GPIO_CONFIG_H_

#define PORT1				1
#define	PORT2				2
#define	PORT3				3

#define PIN0               	0
#define PIN1                1
#define PIN2                2
#define PIN3                3
#define PIN4                4
#define PIN5                5
#define PIN6                6
#define PIN7               	7

#define	LOW					0
#define HIGH				1
#define TOGGLE				2

#define GPIO            	0
#define PRIMARY         	1
#define SECONDARY       	3

#define R_DISABLED   		0
#define R_PULLUP     		1
#define R_PULLDOWN  		2

#define IRQ_NO				0
#define IRQ_YES				1

#define EDGE_NONE			0
#define EDGE_HIGH2LOW      	0
#define EDGE_LOW2HIGH		1

void putc ( void* p, char c);
void conf_clock_16MHz(void);
void conf_calibrate(void);
void conf_pin_function(	unsigned char port,
						unsigned char pin,
						unsigned char pxdir,
						unsigned char pxsel,
						unsigned char pxsel2);
void conf_pin_as_output(unsigned char port, unsigned char pin);
void conf_pin_output(unsigned char port, unsigned char pin, unsigned char state);
void conf_pin_as_input(unsigned char port, \
					   unsigned char pin, \
					   unsigned char resistor, \
					   unsigned char irq, \
					   unsigned char edge_select);
void conf_pin_irq_enable(unsigned char port, unsigned char pin, unsigned char edge_select);
void conf_pin_irq_disable(unsigned char port, unsigned char pin);
unsigned char get_pin_irq_flags_and_clear(unsigned char port);

#endif /* GPIO_CONFIG_H_ */
