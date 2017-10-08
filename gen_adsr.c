#include <msp430.h>
#include <stdint.h>
#include "globals.h"
#include "set_timer.h"
#include "gen_adsr_dividers.h"
#include "gen_adsr_releases.h"

void envelope_adsr_init(struct gen_adsr_struct *adsr) {
	adsr->stage = RELEASE;
	adsr->curve = LINEAR;
	adsr->level = 0;
}

void decay(struct gen_adsr_struct *adsr) {						// decay
	if( adsr->gate == GATE_ON ) {
		adsr->sustain_level = (long signed int) adsr->sustain_level_parameter;
		adsr->sustain_level = adsr->sustain_level * 65536;
		adsr->sustain_level = adsr->sustain_level * 16;
		if( ( adsr->level - adsr_dividers[adsr->decay_step_parameter] ) > adsr->sustain_level + 0xFFFFF ) {
			adsr->level -= adsr_dividers[adsr->decay_step_parameter];
		} else {
			adsr->level = adsr->sustain_level;
		}
	} else {
		adsr->stage = RELEASE;
	}
}

void attack(struct gen_adsr_struct *adsr) {						// attack
	if (adsr->gate == GATE_ON) {
		if ((adsr->level + adsr_dividers[adsr->attack_step_parameter]) >= ENV_MAX) {
			adsr->level = ENV_MAX;
			adsr->stage = DECAY;
			return;
		} else {
			adsr->level += adsr_dividers[adsr->attack_step_parameter];
		}
	} else {
		//adsr->level = adsr_attacks[adsr->level >> (14 + 6)] * 65536 * 8 * 2;
		adsr->stage = RELEASE;
	}
}

void release(struct gen_adsr_struct *adsr) {					// release
	if (adsr->gate == GATE_OFF) {
		if ((adsr->level - adsr_dividers[adsr->release_step_parameter]) > 0) {
			adsr->level -= adsr_dividers[adsr->release_step_parameter];
		} else {
			adsr->level = 0;
		}
	} else {
		adsr->stage = ATTACK;
	}
}

uint16_t envelope(struct gen_adsr_struct *adsr) {
	if ((P2IN & BIT0)) {
		adsr->gate = GATE_ON;
	} else {
		adsr->gate = GATE_OFF;
	}

	switch (adsr->stage) {
	case ATTACK:
		attack(adsr);
		break;
	case DECAY:
		decay(adsr);
		break;
	case RELEASE:
		release(adsr);
		break;
	}

	if( adsr->curve != LINEAR && adsr->stage == ATTACK ) {
		adsr->curve = LINEAR;
		adsr->level = adsr->level4register;
		adsr->level = adsr->level << 14;
		adsr->level = adsr->level << 6;
	}

	if( adsr->curve != CONVEX && (adsr->stage == DECAY || adsr->stage == RELEASE) ) {
		adsr->curve = CONVEX;
		int index, best_index, offset, best_offset;
		best_offset = 1024;
		for(index = 0; index < 1024; index++) {
			offset = abs(adsr_releases[index] - adsr->level4register);
			if(offset < best_offset) {
				best_offset = offset;
				best_index = index;
			}
		}

		adsr->level = best_index;
		adsr->level = adsr->level << 14;
		adsr->level = adsr->level << 6;
	}

	if( adsr->stage == ATTACK ) {
		adsr->level4register = adsr->level >> (14 + 6);
	} else {
		adsr->level4register = adsr_releases[adsr->level >> (14 + 6)];
	}
	return(adsr->level4register);
}
