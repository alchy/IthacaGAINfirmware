/*
 * set_timer.c
 *
 *  Created on: 1. 3. 2016
 *      Author: Jindra
 */

#include "globals.h"
#include "correction.h"

extern unsigned int virt_T0CCR1;
extern unsigned int virt_T0CCR2;
extern unsigned int virt_T1CCR1;
extern unsigned int virt_T1CCR2;

// 10bit PWM timer 0 - 1023
void timer0_set_pwm0(unsigned int ccr) {
	ccr = correction[ccr];
	virt_T0CCR1 = PWMBASE - ccr;
}

// 10bit PWM timer 0 - 1023
void timer0_set_pwm1(unsigned int ccr) {
	ccr = correction[ccr];
	virt_T0CCR2 = PWMBASE - ccr;
}

// 10bit PWM timer 0 - 1023
void timer1_set_pwm0(unsigned int ccr) {
	ccr = correction[ccr];
	virt_T1CCR1 = PWMBASE - ccr;
}

// 10bit PWM timer 0 - 1023
void timer1_set_pwm1(unsigned int ccr) {
	ccr = correction[ccr];
	virt_T1CCR2 = PWMBASE - ccr;
}
