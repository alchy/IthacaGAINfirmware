/*
 * sine.c
 *
 *  Created on: 18. 3. 2016
 *      Author: Jindra
 */

#include <msp430.h>
#include <stdint.h>
#include "globals.h"
#include "gen_sine.h"
#include "gen_table_sine.h"
#include "set_timer.h"

extern	uint16_t	i2c_reg[];

// @ inputs: position in wave 0 - 2047
int16_t gen_sine(uint16_t position) {
   if(position < 1024) {
       if(position < 512) {						// Q1
           return(sinetable[position]);
       } else {									// Q2
           position = 511 + (512 - position);
           return( -1 * sinetable[position]);
       }
   } else {										// Q3
       if(position < 1024 + 512) {
           position = -1 * (1024 - position);
           return( -1 * sinetable[position]);
       } else {									// Q4
           position = 1023 + 1024 - position;
           return(sinetable[position]);
       }
   }
}

int16_t sine(struct gen_lfo_struct *lfo) {
    // @ input offset: 		0 - 1023 /10bit/
    // @ input speed: 		0 - 1023 /10bit/
    // @ input modulation: 	0 - 1023 /10bit/
    // @ function output: 	0 - 1023 /10bit/
    int32_t sine;

    lfo->pos += lfo->speed;					    //        98765432109876543210
    if(lfo->pos > 0x3FFFF) {					// > than 00111111111111111111
    	lfo->pos -= 0x3FFFF;
    }

    lfo->sine_t_pos = lfo->pos > 8;				// convert to 11bit from 19bit number (32bit register)
    sine = gen_sine(lfo->sine_t_pos);			// get sine value from 11bit table
    sine = sine + 1024;
    sine = sine * (1 + lfo->modulation);
    sine = sine >> 11;
    sine = sine + lfo->offset;

    if(sine < 1024) {							// limit output from 0 to 1023
        if(sine < 0) {
            return((uint16_t) 0);
        } else {
            return((uint16_t) sine);
        }
    } else {
        return((uint16_t) 1023);
    }
}
