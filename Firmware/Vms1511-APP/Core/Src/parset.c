/*
 * parset.c
 *
 *		Load/Gain settings driver
 *  	Created on: Sep 19, 2024
 *      Author: Martin Stastny
 */

/* Includes ------------------------------------------------------------------*/
#include "parset.h"

/* Private defines -----------------------------------------------------------*/
/* Private macros  -----------------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
void setExpRegAddr()	// Set I2C expanders register bank
{
	uint8_t data_buffer[2];
	data_buffer[0] = 0;
	data_buffer[1] = 0;
	uint16_t expander_address = I2CEXPANDERADDRESS;
	HAL_I2C_Master_Transmit(&hi2c2, expander_address, data_buffer,
			sizeof(data_buffer), 500);
}


void checkNewPar(amp_gain_t *current_amp_gain, amp_load_t *current_amp_load, conf_reg_t *conf)		// Check if new setting available
{
	if ((*current_amp_gain != conf->amp.gain) || (*current_amp_load != conf->amp.load))
	{
		setNewPar(current_amp_gain,current_amp_load,conf);
	}
}
void setNewPar(amp_gain_t *current_amp_gain, amp_load_t *current_amp_load, conf_reg_t *conf)		// Set new parameters
{
	*current_amp_gain = conf->amp.gain;
	*current_amp_load = conf->amp.load;
	uint8_t data_buffer[2];
	uint16_t expander_address = I2CEXPANDERADDRESS;
	//uint16_t gpioaaddress = I2CGPIOA;

	data_buffer[0] = I2CGPIOA;
	data_buffer[1] = (1u << (*current_amp_load)) | (1u << (*current_amp_gain + 4));
	HAL_I2C_Master_Transmit(&hi2c2, expander_address, data_buffer,sizeof(data_buffer), 500);

}
/* Private Functions ---------------------------------------------------------*/

/** @} */
