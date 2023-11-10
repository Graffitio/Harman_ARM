/*
 * motor.h
 *
 *  Created on: Oct 23, 2023
 *      Author: Wooseong Jung
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "tim.h"

void Forward();
void Backward();
void Left();
void Right();
void Stop();
void motor_control(char *buf);

#endif /* INC_MOTOR_H_ */
