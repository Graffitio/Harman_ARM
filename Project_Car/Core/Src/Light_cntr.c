/*
 * Light_cntr.c
 *
 *  Created on: Nov 10, 2023
 *      Author: Wooseong Jung
 */

#include "Light_cntr.h"

uint16_t lum;

void ADC_start()
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	lum = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
//	HAL_Delay(100);
}

void Light()
{
	if((lum > 800) && (lum < 2500))
	{
		HAL_GPIO_WritePin(Front_Low_Lamp_GPIO_Port, Front_Low_Lamp_Pin, 1);
		HAL_GPIO_WritePin(Front_High_Lamp_GPIO_Port, Front_High_Lamp_Pin, 0);
	}
	else if(lum < 800)
	{
		HAL_GPIO_WritePin(Front_Low_Lamp_GPIO_Port, Front_Low_Lamp_Pin, 1);
		HAL_GPIO_WritePin(Front_High_Lamp_GPIO_Port, Front_High_Lamp_Pin, 1);
	}
	else if(lum > 2500)
	{
		HAL_GPIO_WritePin(Front_Low_Lamp_GPIO_Port, Front_Low_Lamp_Pin, 0);
		HAL_GPIO_WritePin(Front_High_Lamp_GPIO_Port, Front_High_Lamp_Pin, 0);
	}

}
