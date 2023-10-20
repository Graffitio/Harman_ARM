/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "delay.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRIG_L_PORT GPIOB
#define TRIG_L_PIN GPIO_PIN_2
#define TRIG_M_PORT GPIOC
#define TRIG_M_PIN GPIO_PIN_5
#define TRIG_R_PORT GPIOC
#define TRIG_R_PIN GPIO_PIN_4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t INC_Value_L1 = 0; // Echo High?��?�� ?���?
uint32_t INC_Value_L2 = 0; // Echo Low ?��?�� ?���?
uint32_t echoTime_L = 0; // Distance(?���? 거리)�? 측정?���? ?��?�� Echo Time()
uint8_t captureFlag_L = 0;
uint8_t distance_L = 0;

uint32_t INC_Value_M1 = 0; // Echo High?��?�� ?���?
uint32_t INC_Value_M2 = 0; // Echo Low ?��?�� ?���?
uint32_t echoTime_M = 0; // Distance(?���? 거리)�? 측정?���? ?��?�� Echo Time()
uint8_t captureFlag_M = 0;
uint8_t distance_M = 0;

uint32_t INC_Value_R1 = 0; // Echo High?��?�� ?���?
uint32_t INC_Value_R2 = 0; // Echo Low ?��?�� ?���?
uint32_t echoTime_R = 0; // Distance(?���? 거리)�? 측정?���? ?��?�� Echo Time()
uint8_t captureFlag_R = 0;
uint8_t distance_R = 0;

char buf[1];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void HCSR04_Read(void)
{
	HAL_GPIO_WritePin(TRIG_L_PORT, TRIG_L_PIN, 1); // Trig Pin High
	HAL_GPIO_WritePin(TRIG_M_PORT, TRIG_M_PIN, 1); // Trig Pin High
	HAL_GPIO_WritePin(TRIG_R_PORT, TRIG_R_PIN, 1); // Trig Pin High
	delay_us(10);							   // Delay 10us
	HAL_GPIO_WritePin(TRIG_L_PORT, TRIG_L_PIN, 0); // Trig Pin High
	HAL_GPIO_WritePin(TRIG_M_PORT, TRIG_M_PIN, 0); // Trig Pin High
	HAL_GPIO_WritePin(TRIG_R_PORT, TRIG_R_PIN, 0); // Trig Pin High

	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC1); // Set Timer Enable
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC2); // Set Timer Enable
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC3); // Set Timer Enable
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) // 만약 ?��?��?��?�� ?��?�� 채널?���?,(?��리�? 1�? 채널?�� ?��기로 ?��?��)
	{
		if(captureFlag_L == 0) // �? 번째 캡쳐�? ?���? ?��?��?���?,(Falling Edge�? 발생?���? ?��?��?���?,)
		{
			INC_Value_L1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // �? 번째 Value�? ?��?��?��.
			captureFlag_L = 1; // �? 번째 캡쳐 ?���?

			// Polarity �?�?(Rising -> Falling)
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING); // Echo Falling?�� 캡쳐?��?�� ?���?�?
		}
		else if(captureFlag_L == 1) // 만약 �? 번�?? 캡쳐�? ?���? ?��료됐?���?,
		{
			INC_Value_L2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(&htim3, 0); // reset Counter

			if(INC_Value_L2 > INC_Value_L1)
			{
				echoTime_L = INC_Value_L2 - INC_Value_L1;
			}
			else if(INC_Value_L1 > INC_Value_L2) // 거리�? ?���? 길면, INC_Value2 �? Overflow?��?�� count�? 0?���? ?�� ?��?��?��?�� INC_Value1보다 ?��?���? ?�� ?��?��.
			{
				echoTime_L = (0xffff - INC_Value_L1) + INC_Value_L2;
			}
			distance_L = echoTime_L / 58;
			captureFlag_L = 0;

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING); // ?��?�� Echo High�? 캡쳐?��?���? ?��?��
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC1);
		}
	}
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) // 만약 ?��?��?��?�� ?��?�� 채널?���?,(?��리�? 1�? 채널?�� ?��기로 ?��?��)
	{
		if(captureFlag_M == 0) // �? 번째 캡쳐�? ?���? ?��?��?���?,(Falling Edge�? 발생?���? ?��?��?���?,)
		{
			INC_Value_M1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); // �? 번째 Value�? ?��?��?��.
			captureFlag_M = 1; // �? 번째 캡쳐 ?���?

			// Polarity �?�?(Rising -> Falling)
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING); // Echo Falling?�� 캡쳐?��?�� ?���?�?
		}
		else if(captureFlag_M == 1) // 만약 �? 번�?? 캡쳐�? ?���? ?��료됐?���?,
		{
			INC_Value_M2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			__HAL_TIM_SET_COUNTER(&htim3, 0); // reset Counter

			if(INC_Value_M2 > INC_Value_M1)
			{
				echoTime_M = INC_Value_M2 - INC_Value_M1;
			}
			else if(INC_Value_M1 > INC_Value_M2) // 거리�? ?���? 길면, INC_Value2 �? Overflow?��?�� count�? 0?���? ?�� ?��?��?��?�� INC_Value1보다 ?��?���? ?�� ?��?��.
			{
				echoTime_M = (0xffff - INC_Value_M1) + INC_Value_M2;
			}
			distance_M = echoTime_M / 58;
			captureFlag_M = 0;

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING); // ?��?�� Echo High�? 캡쳐?��?���? ?��?��
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC2);
		}
	}
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) // 만약 ?��?��?��?�� ?��?�� 채널?���?,(?��리�? 1�? 채널?�� ?��기로 ?��?��)
	{
		if(captureFlag_R == 0) // �? 번째 캡쳐�? ?���? ?��?��?���?,(Falling Edge�? 발생?���? ?��?��?���?,)
		{
			INC_Value_R1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // �? 번째 Value�? ?��?��?��.
			captureFlag_R = 1; // �? 번째 캡쳐 ?���?

			// Polarity �?�?(Rising -> Falling)
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING); // Echo Falling?�� 캡쳐?��?�� ?���?�?
		}
		else if(captureFlag_R == 1) // 만약 �? 번�?? 캡쳐�? ?���? ?��료됐?���?,
		{
			INC_Value_R2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			__HAL_TIM_SET_COUNTER(&htim3, 0); // reset Counter

			if(INC_Value_R2 > INC_Value_R1)
			{
				echoTime_R = INC_Value_R2 - INC_Value_R1;
			}
			else if(INC_Value_R1 > INC_Value_R2) // 거리�? ?���? 길면, INC_Value2 �? Overflow?��?�� count�? 0?���? ?�� ?��?��?��?�� INC_Value1보다 ?��?���? ?�� ?��?��.
			{
				echoTime_R = (0xffff - INC_Value_R1) + INC_Value_R2;
			}
			distance_R = echoTime_R / 58;
			captureFlag_R = 0;

			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING); // ?��?�� Echo High�? 캡쳐?��?���? ?��?��
			__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC3);
		}
	}
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
  HAL_UART_Receive_DMA(&huart1, buf, sizeof(buf));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HCSR04_Read();
	  printf("L : %d, M : %d, R : %d\r\n", distance_L, distance_M, distance_R);
	  HAL_Delay(200);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */