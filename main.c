#include <msp430.h>
#include <stdint.h>

#include "conf_gpio.h"
#include "globals.h"
#include "setup.h"
#include "i2c.h"
#include "set_timer.h"
#include "gen_sine.h"
#include "gen_adsr.h"

struct gen_adsr_struct adsr0, adsr1, adsr2, adsr3; 				// adsr and lfo control structs
struct gen_lfo_struct lfo0, lfo1, lfo2, lfo3;					// adsr and lfo control structs
extern uint16_t i2c_reg[];										// extern i2c registers
extern uint8_t unit_mode;										// mode of unit detected while boot

unsigned int virt_T0CCR1;										// timer global helper
unsigned int virt_T0CCR2;										// timer global helper
unsigned int virt_T1CCR1;										// timer global helper
unsigned int virt_T1CCR2;										// timer global helper

uint8_t processing_scheduler;									// processing_scheduler

#pragma vector=WDT_VECTOR										// watchdog, each 8000 cycles (2kHz), cca 7x probehne pwm signal
__interrupt void watchdog_timer(void) {
	processing_scheduler++;
}																// WDTIFG is cleared automatically as interrupt is processed in interval mode

#pragma vector=TIMER0_A0_VECTOR									// timer0 irq
__interrupt void TIMER0_A0_ISR_HOOK(void) {
	TA0CCR1 = virt_T0CCR1;
	TA0CCR2 = virt_T0CCR2;
}

#pragma vector=TIMER1_A0_VECTOR									// timer1 irq
__interrupt void TIMER1_A0_ISR_HOOK(void) {
	TA1CCR1 = virt_T1CCR1;
	TA1CCR2 = virt_T1CCR2;
}

int main(void) {												// loop
	setup();
	// debug
	//conf_pin_as_output(PORT1, PIN5);

	if (unit_mode == UNIT_MODE_FILTER) {						// init gain unit as the filter out mixer
		write_register(SYSEX_W_FI_GAIN0_MODE, MODE_HOLD);
		write_register(SYSEX_W_FI_GAIN1_MODE, MODE_HOLD);
		write_register(SYSEX_W_FI_GAIN2_MODE, MODE_HOLD);
		write_register(SYSEX_W_FI_GAIN3_MODE, MODE_ADSR);

		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_A, 512);
		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_C, 0);
		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_D, 0);

		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_A, 512);
		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_C, 0);
		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_D, 0);

		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_A, 0);
		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_C, 0);
		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_D, 0);

		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_A, 0);
		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_C, 1023);
		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_D, 0);

		envelope_adsr_init(&adsr3);
	}

	if (unit_mode == UNIT_MODE_WAVESHAPER) {					// init gain unit as the waveshaper mixer
		write_register(SYSEX_W_FI_GAIN0_MODE, MODE_HOLD);
		write_register(SYSEX_W_FI_GAIN1_MODE, MODE_HOLD);
		write_register(SYSEX_W_FI_GAIN2_MODE, MODE_HOLD);
		write_register(SYSEX_W_FI_GAIN3_MODE, MODE_HOLD);

		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_A, 1023);
		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_C, 0);
		write_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_D, 0);

		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_A, 0);
		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_C, 0);
		write_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_D, 0);

		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_A, 0);
		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_C, 0);
		write_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_D, 0);

		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_A, 0);
		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_B, 0);
		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_C, 0);
		write_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_D, 0);
	}

	for (;;) {
		//P1OUT &= ~BIT5;
		// switch pin off - if the pin is of, we are idle waiting
		// waits need to be the same frequency to maintain synchronicity
		while(processing_scheduler != 4);
		processing_scheduler = 0;
		// switch pin on
		//P1OUT |= BIT5;
		switch (read_register(SYSEX_W_FI_GAIN0_MODE)) {
		case MODE_HOLD:
			timer0_set_pwm0(read_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_A));
			break;
		case MODE_SINE:
			lfo0.offset = read_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_A);
			lfo0.speed = read_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_B);
			lfo0.modulation = read_register(SYSEX_W_FI_GAIN0_VIRTUAL_REG_C);
			timer0_set_pwm0(sine(&lfo0));
			break;
		case MODE_ADSR:
			adsr0.attack_step_parameter = read_register(
			SYSEX_W_FI_GAIN0_VIRTUAL_REG_A);
			adsr0.decay_step_parameter = read_register(
			SYSEX_W_FI_GAIN0_VIRTUAL_REG_B);
			adsr0.sustain_level_parameter = read_register(
			SYSEX_W_FI_GAIN0_VIRTUAL_REG_C);
			adsr0.release_step_parameter = read_register(
			SYSEX_W_FI_GAIN0_VIRTUAL_REG_D);
			timer0_set_pwm0(envelope(&adsr0));
			break;
		};

		switch (read_register(SYSEX_W_FI_GAIN1_MODE)) {
		case MODE_HOLD:
			timer0_set_pwm1(read_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_A));
			break;
		case MODE_SINE:
			lfo1.offset = read_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_A);
			lfo1.speed = read_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_B);
			lfo1.modulation = read_register(SYSEX_W_FI_GAIN1_VIRTUAL_REG_C);
			timer0_set_pwm1(sine(&lfo1));
			break;
		case MODE_ADSR:
			adsr1.attack_step_parameter = read_register(
			SYSEX_W_FI_GAIN1_VIRTUAL_REG_A);
			adsr1.decay_step_parameter = read_register(
			SYSEX_W_FI_GAIN1_VIRTUAL_REG_B);
			adsr1.sustain_level_parameter = read_register(
			SYSEX_W_FI_GAIN1_VIRTUAL_REG_C);
			adsr1.release_step_parameter = read_register(
			SYSEX_W_FI_GAIN1_VIRTUAL_REG_D);
			timer0_set_pwm1(envelope(&adsr1));
			break;
		};

		switch (read_register(SYSEX_W_FI_GAIN2_MODE)) {
		case MODE_HOLD:
			timer1_set_pwm0(read_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_A));
			break;
		case MODE_SINE:
			lfo2.offset = read_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_A);
			lfo2.speed = read_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_B);
			lfo2.modulation = read_register(SYSEX_W_FI_GAIN2_VIRTUAL_REG_C);
			timer1_set_pwm0(sine(&lfo2));
			break;
		case MODE_ADSR:
			adsr2.attack_step_parameter = read_register(
			SYSEX_W_FI_GAIN2_VIRTUAL_REG_A);
			adsr2.decay_step_parameter = read_register(
			SYSEX_W_FI_GAIN2_VIRTUAL_REG_B);
			adsr2.sustain_level_parameter = read_register(
			SYSEX_W_FI_GAIN2_VIRTUAL_REG_C);
			adsr2.release_step_parameter = read_register(
			SYSEX_W_FI_GAIN2_VIRTUAL_REG_D);
			timer1_set_pwm0(envelope(&adsr2));
			break;
		};

		switch (read_register(SYSEX_W_FI_GAIN3_MODE)) {
		case MODE_HOLD:
			timer1_set_pwm1(read_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_A));
			break;
		case MODE_SINE:
			lfo3.offset = read_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_A);
			lfo3.speed = read_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_B);
			lfo3.modulation = read_register(SYSEX_W_FI_GAIN3_VIRTUAL_REG_C);
			timer1_set_pwm1(sine(&lfo3));
			break;
		case MODE_ADSR:
			adsr3.attack_step_parameter = read_register(
			SYSEX_W_FI_GAIN3_VIRTUAL_REG_A);
			adsr3.decay_step_parameter = read_register(
			SYSEX_W_FI_GAIN3_VIRTUAL_REG_B);
			adsr3.sustain_level_parameter = read_register(
			SYSEX_W_FI_GAIN3_VIRTUAL_REG_C);
			adsr3.release_step_parameter = read_register(
			SYSEX_W_FI_GAIN3_VIRTUAL_REG_D);
			timer1_set_pwm1(envelope(&adsr3));
			break;
		}
	}
}
