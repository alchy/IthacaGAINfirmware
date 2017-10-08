/*
 * i2c.h
 *
 *  Created on: 1. 3. 2016
 *      Author: Jindra
 */

#ifndef I2C_H_
#define I2C_H_

__interrupt void USCI0TX_ISR_HOOK(void);
__interrupt void USCI0RX_ISR_HOOK(void);

void 		I2c_Init(void);
void 		write_register(uint8_t i2c_reg_addr, uint16_t val);
uint16_t 	read_register(uint8_t i2c_reg_addr);

#endif /* I2C_H_ */
