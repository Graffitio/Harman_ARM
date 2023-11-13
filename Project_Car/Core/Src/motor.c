/*
 * motot.c
 *
 *  Created on: Oct 23, 2023
 *      Author: Wooseong Jung
 */
#include "motor.h"
#include "main.h"
#include "I2C_LCD.h"

void motor_control(char *buf)
{
	  if(buf[0] != 0)
		{
			switch(buf[0])
			{
			case 'w' :
				Forward();
				htim10.Instance->CCR1 = 98; // Left
				htim11.Instance->CCR1 = 98; // Right
				HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
//				printf("Forward \r\n");
//				lcd_put_cur(1, 0);
//				lcd_send_string("Forward ");
				break;
			case 's' :
				Backward();
				htim10.Instance->CCR1 = 98; // Left
				htim11.Instance->CCR1 = 98; // Right
				HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
//				HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 1);
//				HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 1);
//				printf("Backward \r\n");
//				lcd_put_cur(1, 0);
//				lcd_send_string("Backward");
				break;
			case 'a' :
//				Forward();
//				htim10.Instance->CCR1 = 60; // Left
//				htim11.Instance->CCR1 = 98; // Right
				Left();
				htim10.Instance->CCR1 = 98; // Left
				htim11.Instance->CCR1 = 98; // Right
//				HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
//				HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 1);
//				printf("Left \r\n");
//				lcd_put_cur(1, 0);
//				lcd_send_string("Left    ");
				break;
			case 'd' :
//				Forward();
//				htim10.Instance->CCR1 = 98; // Left
//				htim11.Instance->CCR1 = 60; // Right
				Right();
				htim10.Instance->CCR1 = 98; // Left
				htim11.Instance->CCR1 = 98; // Right
//				HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
//				HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 1);
//				printf("Right \r\n");
//				lcd_put_cur(1, 0);
//				lcd_send_string("Right   ");
				break;
			case 'z' :
				Stop();
//				htim10.Instance->CCR1 = 10000; // Left
//				htim11.Instance->CCR1 = 7500; // Right
//				printf("Right \r\n");
//				lcd_put_cur(1, 0);
//				lcd_send_string("Right   ");
//				HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 1);
				break;
			default :
				printf("Error \r\n");
				break;
			}
		}
//		buf[0] = 0;
//	  	HAL_Delay(50);
}


void Forward()
{
	HAL_GPIO_WritePin(Right_Front_GPIO_Port, Right_Front_Pin, 1);
	HAL_GPIO_WritePin(Right_Back_GPIO_Port, Right_Back_Pin, 0);
	HAL_GPIO_WritePin(Left_Front_GPIO_Port, Left_Front_Pin, 0);
	HAL_GPIO_WritePin(Left_Back_GPIO_Port, Left_Back_Pin, 1);

	HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
	HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 0);
	HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 0);
}

void Left()
{
	HAL_GPIO_WritePin(Right_Front_GPIO_Port, Right_Front_Pin, 1);
	HAL_GPIO_WritePin(Right_Back_GPIO_Port, Right_Back_Pin, 0);
	HAL_GPIO_WritePin(Left_Front_GPIO_Port, Left_Front_Pin, 1);
	HAL_GPIO_WritePin(Left_Back_GPIO_Port, Left_Back_Pin, 0);

	HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
	HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 1);
	HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 0);
}

void Right()
{
	HAL_GPIO_WritePin(Right_Front_GPIO_Port, Right_Front_Pin, 0);
	HAL_GPIO_WritePin(Right_Back_GPIO_Port, Right_Back_Pin, 1);
	HAL_GPIO_WritePin(Left_Front_GPIO_Port, Left_Front_Pin, 0);
	HAL_GPIO_WritePin(Left_Back_GPIO_Port, Left_Back_Pin, 1);

	HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
	HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 0);
	HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 1);
}

void Backward()
{
	HAL_GPIO_WritePin(Right_Front_GPIO_Port, Right_Front_Pin, 0);
	HAL_GPIO_WritePin(Right_Back_GPIO_Port, Right_Back_Pin, 1);
	HAL_GPIO_WritePin(Left_Front_GPIO_Port, Left_Front_Pin, 1);
	HAL_GPIO_WritePin(Left_Back_GPIO_Port, Left_Back_Pin, 0);

	HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 1);
	HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 1);
}

void Stop()
{
	HAL_GPIO_WritePin(Right_Front_GPIO_Port, Right_Front_Pin, 0);
	HAL_GPIO_WritePin(Right_Back_GPIO_Port, Right_Back_Pin, 0);
	HAL_GPIO_WritePin(Left_Front_GPIO_Port, Left_Front_Pin, 0);
	HAL_GPIO_WritePin(Left_Back_GPIO_Port, Left_Back_Pin, 0);

	HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 1);
	HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 0);
	HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 0);
}
