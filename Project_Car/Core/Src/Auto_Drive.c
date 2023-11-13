/*
 * Auto_Drive.c
 *
 *  Created on: Nov 10, 2023
 *      Author: Wooseong Jung
 */

#include "Auto_Drive.h"

uint8_t dist_buff[30];
extern char buf[1];

void Auto_Drive_01()
{
	  extern uint8_t distance_L;
	  extern uint8_t distance_M;
	  extern uint8_t distance_R;
	  extern uint8_t mode_cnt;
	  extern uint8_t mode;
	  uint8_t mode_cnt_lcd = HAL_GPIO_ReadPin(Blue_button_GPIO_Port, Blue_button_Pin);
	  sprintf(dist_buff, "L%-3d M%-3d R%-3d", distance_L, distance_M, distance_R);
	  lcd_put_cur(1, 0);
	  lcd_send_string(dist_buff);
	  motor_control(&buf);
	  if(mode == 0)
	  {
//		  if(mode_cnt_lcd == 0) Stop();
//		  lcd_put_cur(1, 0);
//		  lcd_send_string("  Manual Drive  ");
		  motor_control(&buf);
	  }
	  else
	  {
		  // distance_L, distance_M, distance_R
		  if(mode_cnt_lcd == 0) Stop();
//		lcd_put_cur(1, 0);
//		lcd_send_string("   Auto Drive   ");
		  if(distance_M >= LIMIT_F)
		  {
			  if((distance_L > LIMIT_D) && (distance_R > LIMIT_D))
			  {
				  Forward();
				  htim10.Instance->CCR1 = LIMIT_SPEED_R; // Left
				  htim11.Instance->CCR1 = LIMIT_SPEED_R; // Right
			  }
			  else if(distance_L < LIMIT_D)
			  {
				  Stop();
				  Forward();
				  htim10.Instance->CCR1 = 98; // Left
				  htim11.Instance->CCR1 = 63; // Right
				  HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
				  HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 0);
				  HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 1);
//				  Stop();
//				  Right();
//				  htim10.Instance->CCR1 = LIMIT_SPEED_T; // Left
//				  htim11.Instance->CCR1 = LIMIT_SPEED_T; // Right
//				  Right();
////				  htim10.Instance->CCR1 = 4000; // Left
////				  htim11.Instance->CCR1 = 3000; // Right
//				  htim10.Instance->CCR1 = 8000; // Left
//				  htim11.Instance->CCR1 = 8000; // Right-
//				  state = 3;
//			  delay_us(10000);
			  }
			  else if(distance_R < LIMIT_D)
			  {
				  Stop();
				  Forward();
				  htim10.Instance->CCR1 = 63; // Left
				  htim11.Instance->CCR1 = 98; // Right
				  HAL_GPIO_WritePin(Back_Red_Lamp_GPIO_Port, Back_Red_Lamp_Pin, 0);
				  HAL_GPIO_WritePin(Back_Blue_Right_Lamp_GPIO_Port, Back_Blue_Right_Lamp_Pin, 1);
				  HAL_GPIO_WritePin(Back_Blue_Left_Lamp_GPIO_Port, Back_Blue_Left_Lamp_Pin, 0);
//				  Stop();
//				  Left();
//				  htim10.Instance->CCR1 = LIMIT_SPEED_T; // Left
//				  htim11.Instance->CCR1 = LIMIT_SPEED_T; // Right
//				  Left();
//				  htim10.Instance->CCR1 = 8000; // Left-
//				  htim11.Instance->CCR1 = 8000; // Right
//			  delay_us(10000);
			  }
//			  else if((distance_L < 28) && (distance_R > 28))
//			  {
//				  Stop();
//				  Forward();
//				  htim10.Instance->CCR1 = 63; // Left
//				  htim11.Instance->CCR1 = 98; // Right
//			  }
//			  else if((distance_L > 28) && (distance_R < 28))
//			  {
//				  Stop();
//				  Forward();
//				  htim10.Instance->CCR1 = 98; // Left
//				  htim11.Instance->CCR1 = 63; // Right
//			  }
		  }
		  else if(distance_M < LIMIT_F)
		  {
//			  Forward();
			  Stop();
			  htim10.Instance->CCR1 = LIMIT_SPEED_R; // Left
			  htim11.Instance->CCR1 = LIMIT_SPEED_R; // Right
////			  delay_us(50000);
			  if((distance_L - distance_R) <= 0)
			  {
//				  Right();
//				  htim10.Instance->CCR1 = 100; // Left
//				  htim11.Instance->CCR1 = 100; // Right-
//				  Forward();
//				  htim10.Instance->CCR1 = 98; // Left
//				  htim11.Instance->CCR1 = 60; // Right
				  Right();
				  htim10.Instance->CCR1 = LIMIT_SPEED_T; // Left
				  htim11.Instance->CCR1 = LIMIT_SPEED_T; // Right
			  }
			  else if((distance_L - distance_R) > 0)
			  {
//				  Left();
//				  htim10.Instance->CCR1 = 100; // Left-
//				  htim11.Instance->CCR1 = 100; // Right
//				  Forward();
//				  htim10.Instance->CCR1 = 60; // Left
//				  htim11.Instance->CCR1 = 98; // Right
				  Left();
				  htim10.Instance->CCR1 = LIMIT_SPEED_T; // Left
				  htim11.Instance->CCR1 = LIMIT_SPEED_T; // Right
			  }
		  }
	  }
}
