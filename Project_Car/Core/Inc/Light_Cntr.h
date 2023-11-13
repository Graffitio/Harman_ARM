/*
 * Light_Cntr.h
 *
 *  Created on: Nov 10, 2023
 *      Author: Wooseong Jung
 */

#ifndef INC_LIGHT_CNTR_H_
#define INC_LIGHT_CNTR_H_

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "delay.h"

void ADC_start();
void Light();


#endif /* INC_LIGHT_CNTR_H_ */
