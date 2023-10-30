/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "delay.h"
#include "stdio.h"
#include "ultrasonic.h"
#include "I2C_LCD.h"
#include "motor.h"
#include "usart.h"
#include "dma.h"
#include "i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MANUAL 0
#define LIMIT_D 23
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

char buf[1];
uint8_t mode;
uint8_t lcd_buf[30];
//extern uint8_t distance_L, distance_M, distance_R;
uint8_t state;
extern uint8_t ult_stg;
//char bluetooth_buf[1];

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId Ultra_sonicHandle;
osThreadId Auto_DriveHandle;
osThreadId I2C_LCDHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, &ch, 1, 200);
	return ch;
}

int __io_getchar(void)
{
	int ch; // buf?�� ???��?���???????????????? ?��?��?��, ch?��?�� ???��
	while(1)
	{
		if(HAL_UART_Receive(&huart2, &ch, 1, 200) == HAL_OK)
			break;
	}
	HAL_UART_Transmit(&huart2, &ch, 1, 200);

	return ch;
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Ultra_sonic_x3(void const * argument);
void Auto_Drive01(void const * argument);
void LCD(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
	//  HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_1, buf_echo_1, sizeof(buf_echo_1));
	//  HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_2, buf_echo_2, sizeof(buf_echo_1));
	//  HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_4, buf_echo_4, sizeof(buf_echo_4));
	  HAL_UART_Receive_DMA(&huart1, buf, sizeof(buf));
	  HAL_UART_Receive_DMA(&huart2, buf, sizeof(buf));
	  HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1); // Left
	  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1); // Right

	  HAL_Delay(1000); // ?��?�� ?���??? ?��리니�??? ?��?��?�� 주자.
	  lcd_init();
	  lcd_send_string("Hello HARMAN"); // 12�????��
	  HAL_Delay(500);
	  lcd_put_cur(1, 0); // ?��?�� ?��?���??? 커서 ?��?��
	  lcd_send_string("TEST LCD COM!"); // 12�????��
	  HAL_Delay(1000);
	lcd_clear();
	ult_stg = 1;
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Ultra_sonic */
  osThreadDef(Ultra_sonic, Ultra_sonic_x3, osPriorityIdle, 0, 128);
  Ultra_sonicHandle = osThreadCreate(osThread(Ultra_sonic), NULL);

  /* definition and creation of Auto_Drive */
  osThreadDef(Auto_Drive, Auto_Drive01, osPriorityIdle, 0, 128);
  Auto_DriveHandle = osThreadCreate(osThread(Auto_Drive), NULL);

  /* definition and creation of I2C_LCD */
  osThreadDef(I2C_LCD, LCD, osPriorityIdle, 0, 128);
  I2C_LCDHandle = osThreadCreate(osThread(I2C_LCD), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	  uint8_t mode_cnt = HAL_GPIO_ReadPin(Blue_button_GPIO_Port, Blue_button_Pin);
	  if(mode_cnt == 0)
	  {
		  if(mode < 1)
			  {
			  	  mode++;
			  	  delay_us(10000);
			  }
		  else mode = 0;
	  }
    osDelay(200);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Ultra_sonic_x3 */
/**
* @brief Function implementing the Ultra_sonic thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Ultra_sonic_x3 */
void Ultra_sonic_x3(void const * argument)
{
  /* USER CODE BEGIN Ultra_sonic_x3 */
  /* Infinite loop */
  for(;;)
  {
	  HCSR04_Read();
//	  distance_filter();
//	  print_distance();
    osDelay(60);
  }
  /* USER CODE END Ultra_sonic_x3 */
}

/* USER CODE BEGIN Header_Auto_Drive01 */
/**
* @brief Function implementing the Auto_Drive thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Auto_Drive01 */
void Auto_Drive01(void const * argument)
{
  /* USER CODE BEGIN Auto_Drive01 */
  /* Infinite loop */
  for(;;)
  {
	  extern uint8_t distance_L;
	  extern uint8_t distance_M;
	  extern uint8_t distance_R;
//	  motor_control(&buf);
	  if(mode == 0)
	  {
		  Stop();
		  motor_control(&buf);
	  }
//	  else
//	  {
//		  // distance_L, distance_M, distance_R
//		  if(mode == 0) Stop();
//		  else if((distance_L > LIMIT_D) && (distance_M > LIMIT_D) && (distance_R > LIMIT_D))
//		  {
//			  Forward();
//			  htim10.Instance->CCR1 = 10000; // Left
//			  htim11.Instance->CCR1 = 10000; // Right
//			  state = 1;
//		  }
////		  else if((distance_L < LIMIT_D) || (distance_M > LIMIT_D) || (distance_R > LIMIT_D))
////		  {
////			  Forward();
////			  htim10.Instance->CCR1 = 10000; // Left
////			  htim11.Instance->CCR1 = 8000; // Right
////			  state = 2;
//////			  delay_us(10000);
////		  }
//		  else if((distance_L < LIMIT_D) && (distance_M < LIMIT_D) && (distance_R > LIMIT_D))
//		  {
//			  Forward();
//			  htim10.Instance->CCR1 = 10000; // Left
//			  htim11.Instance->CCR1 = 6000; // Right
//			  state = 3;
////			  delay_us(10000);
//		  }
//		  else if((distance_L < LIMIT_D) && (distance_M < LIMIT_D) && (distance_R < LIMIT_D))
//		  {
//			  Backward();
//			  htim10.Instance->CCR1 = 10000; // Left
//			  htim11.Instance->CCR1 = 10000; // Right
//			  state = 4;
////			  delay_us(10000);
//		  }
////		  else if((distance_L > LIMIT_D) || (distance_M > LIMIT_D) || (distance_R < LIMIT_D))
////		  {
////			  Forward();
////			  htim10.Instance->CCR1 = 8000; // Left
////			  htim11.Instance->CCR1 = 10000; // Right
////			  state = 6;
//////			  delay_us(10000);
////		  }
//		  else if((distance_L > LIMIT_D) && (distance_M < LIMIT_D) && (distance_R < LIMIT_D))
//		  {
//			  Forward();
//			  htim10.Instance->CCR1 = 6000; // Left
//			  htim11.Instance->CCR1 = 10000; // Right
//			  state = 7;
////			  delay_us(10000);
//		  }
//	  }


//	  else
//	  {
//		  // distance_L, distance_M, distance_R
////		  if(mode == 0) Stop();
//		  if((distance_L > LIMIT_D) && (distance_M > LIMIT_D) && (distance_R > LIMIT_D))
//		  {
//			  Forward();
//			  htim10.Instance->CCR1 = 10000; // Left
//			  htim11.Instance->CCR1 = 10000; // Right
//			  state = 1;
//		  }
////		  else if((distance_L < LIMIT_D) || (distance_M > LIMIT_D) || (distance_R > LIMIT_D))
////		  {
////			  Forward();
////			  htim10.Instance->CCR1 = 10000; // Left
////			  htim11.Instance->CCR1 = 8000; // Right
////			  state = 2;
//////			  delay_us(10000);
////		  }
//		  if(distance_L < LIMIT_D)
//		  {
//			  Forward();
//			  htim10.Instance->CCR1 = 10000; // Left
//			  htim11.Instance->CCR1 = 4500; // Right
//			  state = 3;
////			  delay_us(10000);
//		  }
//		  if(distance_M < 20)
//		  {
//			  Backward();
//			  htim10.Instance->CCR1 = 10000; // Left
//			  htim11.Instance->CCR1 = 10000; // Right
//			  if((distance_L - distance_R) > 0)
//			  {
//				  htim10.Instance->CCR1 = 4500; // Left
//				  htim11.Instance->CCR1 = 10000; // Right
//			  }
//			  else if((distance_L - distance_R) <= 0)
//			  {
//				  htim10.Instance->CCR1 = 10000; // Left
//				  htim11.Instance->CCR1 = 4500; // Right
//			  }
//			  state = 4;
////			  delay_us(10000);
//		  }
////		  else if((distance_L < 23) && (distance_M < LIMIT_D) && (distance_R < 23))
////		  {
////			  Backward();
////			  htim10.Instance->CCR1 = 10000; // Left
////			  htim11.Instance->CCR1 = 10000; // Right
////			  state = 5;
//////			  delay_us(10000);
////		  }
////		  else if((distance_L > LIMIT_D) || (distance_M > LIMIT_D) || (distance_R < LIMIT_D))
////		  {
////			  Forward();
////			  htim10.Instance->CCR1 = 8000; // Left
////			  htim11.Instance->CCR1 = 10000; // Right
////			  state = 6;
//////			  delay_us(10000);
////		  }
//		  if(distance_R < LIMIT_D)
//		  {
//			  Forward();
//			  htim10.Instance->CCR1 = 4500; // Left
//			  htim11.Instance->CCR1 = 10000; // Right
//			  state = 7;
////			  delay_us(10000);
//		  }
//	  }

	  else
	  {
		  // distance_L, distance_M, distance_R
//		  if(mode == 0) Stop();
		  if(distance_M > 13)
		  {
			  if((distance_L > LIMIT_D) && (distance_R > LIMIT_D))
			  {
				  Forward();
				  htim10.Instance->CCR1 = 10000; // Left
				  htim11.Instance->CCR1 = 10000; // Right
				  state = 1;
			  }
			  else if(distance_L < LIMIT_D)
			  {
				  Forward();
				  htim10.Instance->CCR1 = 10000; // Left
				  htim11.Instance->CCR1 = 2000; // Right
				  state = 3;
//			  delay_us(10000);
			  }
			  else if(distance_R < LIMIT_D)
			  {
				  Forward();
				  htim10.Instance->CCR1 = 2000; // Left
				  htim11.Instance->CCR1 = 10000; // Right
				  state = 7;
//			  delay_us(10000);
			  }
			  else if((distance_L < LIMIT_D) && (distance_R < LIMIT_D))
			  {
				  Backward();
				  htim10.Instance->CCR1 = 10000; // Left
				  htim11.Instance->CCR1 = 10000; // Right
//				  delay_us(10000);
				  if((distance_L - distance_R) > 0)
				  {
//					  Forward();
					  htim10.Instance->CCR1 = 10000; // Left
					  htim11.Instance->CCR1 = 2000; // Right
				  }
				  else if((distance_L - distance_R) <= 0)
				  {
//					  Forward();
					  htim10.Instance->CCR1 = 2000; // Left
					  htim11.Instance->CCR1 = 10000; // Right
				  }
			  }
		  }
		  else if(distance_M < 13)
		  {
			  Backward();
			  htim10.Instance->CCR1 = 10000; // Left
			  htim11.Instance->CCR1 = 10000; // Right
//			  delay_us(10000);
			  if((distance_L - distance_R) > 0)
			  {
//					  Forward();
				  htim10.Instance->CCR1 = 10000; // Left
				  htim11.Instance->CCR1 = 2000; // Right
			  }
			  else if((distance_L - distance_R) <= 0)
			  {
//					  Forward();
				  htim10.Instance->CCR1 = 2000; // Left
				  htim11.Instance->CCR1 = 10000; // Right
			  }
			  state = 4;
		  }
	  }
    osDelay(200);
  }
  /* USER CODE END Auto_Drive01 */
}

/* USER CODE BEGIN Header_LCD */
/**
* @brief Function implementing the I2C_LCD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LCD */
void LCD(void const * argument)
{
  /* USER CODE BEGIN LCD */
  /* Infinite loop */
  for(;;)
  {
	  lcd_put_cur(0, 0);
	  switch(mode)
	  {
	  	  case 0 :
	  		  lcd_send_string("Manual Mode");
	  		  break;
	  	  case 1 :
	  		  lcd_send_string("  Auto Mode");
	  		  break;
	  }
//	  sprintf(lcd_buf, "Mode : %s", count++);
//	  lcd_send_string(lcd_buf);
//
//	  lcd_put_cur(1, 0);
//	  if(buf[0] != 0)
//		{
//			switch(buf[0])
//			{
//			case 'w' :
//				lcd_send_string(" Forward");
//				break;
//			case 's' :
//				lcd_send_string("Backward");
//				break;
//			case 'a' :
//				lcd_send_string("    Left");
//				break;
//			case 'd' :
//				lcd_send_string("   Right");
//				break;
//			default :
//				printf("Error \r\n");
//				break;
//			}
//		}
    osDelay(300);
  }
  /* USER CODE END LCD */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
