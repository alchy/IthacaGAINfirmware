#include <stdint.h>

#ifndef GLOBALS_H_
#define GLOBALS_H_

// i2c address space description
#define CHAIN_ADDRESS_SPACE_BEGIN           0x18
#define CHAIN_ADDRESS_SPACE_END             CHAIN_ADDRESS_SPACE_BEGIN + 0x20

// base address for DCO->SHAPER->[GAIN]->OUT
#define I2C_BASE_ADDR 						0x18
											// base addres   DCO is 0x08 == 8
											// base address ADSR is 0x18 == 8 + 16 (24)

// ~16Khz refresh rate, Butterworth filter is ~1Khz
// 20 cycles is added as a reserve for irq routine entry
#define	PWMBASE								1023 + 22

// device type request address
#define REG_B_DEVTYPE           			0xfe

// device type response
#define DEV_TYPE_DCO        				0x10
#define DEV_TYPE_FILTER_GAIN				0x11
#define DEV_TYPE_SHAPER_GAIN				0x12

// device position in a chain
#define UNIT_MODE_FILTER					0
#define UNIT_MODE_WAVESHAPER				32

#define SYSEX_RANGE_CHAIN_START				64

#define	MODE_HOLD							0
#define MODE_SINE							1
#define	MODE_ADSR							2

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx this block must be the same as in mbed code xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define	SYSEX_W_FI_GAIN0_MODE			 	64
#define	SYSEX_W_FI_GAIN1_MODE			 	65
#define	SYSEX_W_FI_GAIN2_MODE			 	66
#define	SYSEX_W_FI_GAIN3_MODE			 	67

#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_A		68
#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_B		69
#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_C		70
#define SYSEX_W_FI_GAIN0_VIRTUAL_REG_D		71

#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_A		72
#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_B		73
#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_C		74
#define SYSEX_W_FI_GAIN1_VIRTUAL_REG_D		75

#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_A		76
#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_B		77
#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_C		78
#define SYSEX_W_FI_GAIN2_VIRTUAL_REG_D		79

#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_A		80
#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_B		81
#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_C		82
#define SYSEX_W_FI_GAIN3_VIRTUAL_REG_D		83

#define	SYSEX_W_WS_GAIN0_MODE			 	96
#define	SYSEX_W_WS_GAIN1_MODE			 	97
#define	SYSEX_W_WS_GAIN2_MODE			 	98
#define	SYSEX_W_WS_GAIN3_MODE			 	99

#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_A		100
#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_B		101
#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_C		102
#define SYSEX_W_WS_GAIN0_VIRTUAL_REG_D		103

#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_A		104
#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_B		105
#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_C		106
#define SYSEX_W_WS_GAIN1_VIRTUAL_REG_D		107

#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_A		108
#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_B		109
#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_C		110
#define SYSEX_W_WS_GAIN2_VIRTUAL_REG_D		111

#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_A		112
#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_B		113
#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_C		114
#define SYSEX_W_WS_GAIN3_VIRTUAL_REG_D		115


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// ---------------------------------------------
// internal definitions
// ---------------------------------------------
#define 	ATTACK		0
#define		DECAY		1
#define 	RELEASE		2

#define		LINEAR		0
#define		CONCAVE		1
#define		CONVEX		2

#define		GATE_OFF	0
#define		GATE_ON		1

// ---------------------------------------------
// subrutine declarations
// ---------------------------------------------
//#define		ENV_MAX		0x7FFFFFFF/2
//
#define		ENV_MAX		0x3FFFFFFF
// 0011 1111 1111 1111 1111 1111 1111 1111 / 1024
// == 1 048 575 == 0xFFFFF == 0b1111 1111 1111 1111 1111


// ---------------------------------------------
// structures
// ---------------------------------------------
struct gen_adsr_struct {								// control pannel adsr 1:1 modifiers and internal adsr helpers
	uint8_t 	gate,
				stage,
				curve,
				timer_nr;
	int32_t		attack_step,
				decay_step,
				sustain_level,
				release_step;
	uint16_t	attack_step_parameter,
				decay_step_parameter,
				sustain_level_parameter,
				release_step_parameter,
				level4register;
	int32_t 	level;
};

struct gen_lfo_struct {									// control pannel lfo 1:1 modifiers and internal lfo helpers
	int16_t 	speed,
				offset,
				modulation;
	uint8_t		shape;
	uint16_t 	sine_t_pos;
	uint32_t	pos;
};

#endif /* GLOBALS_H_ */
