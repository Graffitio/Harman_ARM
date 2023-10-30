/*
 * ultrasonic.h
 *
 *  Created on: Oct 23, 2023
 *      Author: Wooseong Jung
 */

#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "main.h"

#define TRIG_PORT GPIOB
#define TRIG_PIN GPIO_PIN_2
#define FILTER_CNT 50




void HCSR04_Read(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void print_distance();
void distance_filter();


#endif /* INC_ULTRASONIC_H_ */
