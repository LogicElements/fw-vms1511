/*
 * parset.h
 *
 *		Load/Gain settings driver
 *  	Created on: Sep 19, 2024
 *      Author: Martin Stastny
 *
 */

#ifndef INC_PARSET_H_
#define INC_PARSET_H_

// Includes
#include "reg_map.h"
#include "i2c.h"

#define I2CEXPANDERADDRESS	64;	// 7 Bit sequence - 0 1 0 0 0 0 0 0 - 0 1 0 0 (fixed) A2 A1 A0 R/W	A2,A1,A0 == 0, R/W == 0 --> Write

// I2C Register bank address
// All for settings IOCON.BANK = 0	(IOCON register byte 7)
#define I2CIOCONADDRESS		0x0A;
#define I2CGPIOA			0x12;
#define I2CGPIOB			0x13;
#define I2CIODIRAADDRESS	0x00;
#define I2CIODIRBADDRESS	0x01;

void setExpRegAddr();	// Set I2C expanders register bank

void checkNewPar(amp_gain_t *current_amp_gain, amp_load_t *current_amp_load, conf_reg_t *conf);		// Check if new setting available

void setNewPar(amp_gain_t *current_amp_gain, amp_load_t *current_amp_load, conf_reg_t *conf);		// Set new parameters


#endif /* INC_PARSET_H_ */
