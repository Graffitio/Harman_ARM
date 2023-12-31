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
#include "cmsis_os.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

osThreadId defaultTaskHandle;
osThreadId Task_LED_RedHandle;
osThreadId Task_LED_GreenHandle;
osThreadId Task_LED_BlueHandle;
/* USER CODE BEGIN PV */
char rxbuf[100];
int cMode; // LED Control : 1, 2, 3(R, G, Y) / 4(All)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);
void Task_LED_R(void const * argument);
void Task_LED_G(void const * argument);
void Task_LED_B(void const * argument);

/* USER CODE BEGIN PFP */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, &ch, 1, 10);
	return ch;
}
int __io_getchar(void)
{
	int ch;
	while(1)
	{
		if(HAL_UART_Receive(&huart2, &ch, 1, 10) == HAL_OK)
		break;
	}
	if (ch == '\b' || ch == 0x7f)
	{
		HAL_UART_Transmit(&huart2, "\b \b", 3, 10); // \b \b : 총 세 글자이기때문에 3
	}
	else if(ch == '\n')
	{
		HAL_UART_Transmit(&huart2, "\r\n", 2, 10);
	}
	else HAL_UART_Transmit(&huart2, &ch, 1, 10);

	return ch;
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  // HAL_UART_Receive_DMA(&huart2, rxbuf, 100);
  // DMA가 두 가지 방식이므로 Receive, Circular DMA에 방식이므로 100까지 쌓이다가 처음부터 다시 쌓임
  // 근데 이렇게 하면 별도로 DMA를 관리하는 기능이 추가로 필요하다.
  // Circular mode이므로 내가 최종적으로 받은 데이터가 어디에 위치하는지 잘 모르게 됨.
  // 그래서 그냥 심플하게 size를 1로 하면 편함
  HAL_UART_Receive_DMA(&huart2, &rxbuf, 1);
  /* USER CODE END 2 */

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

  /* definition and creation of Task_LED_Red */
  osThreadDef(Task_LED_Red, Task_LED_R, osPriorityIdle, 0, 128);
  Task_LED_RedHandle = osThreadCreate(osThread(Task_LED_Red), NULL);

  /* definition and creation of Task_LED_Green */
  osThreadDef(Task_LED_Green, Task_LED_G, osPriorityIdle, 0, 128);
  Task_LED_GreenHandle = osThreadCreate(osThread(Task_LED_Green), NULL);

  /* definition and creation of Task_LED_Blue */
  osThreadDef(Task_LED_Blue, Task_LED_B, osPriorityIdle, 0, 128);
  Task_LED_BlueHandle = osThreadCreate(osThread(Task_LED_Blue), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_Blue_Pin|LD2_Pin|LED_Green_Pin|LED_main_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Blue_Pin LD2_Pin LED_Green_Pin LED_main_Pin */
  GPIO_InitStruct.Pin = LED_Blue_Pin|LD2_Pin|LED_Green_Pin|LED_main_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int state = 0; // Input state : 0 / ready , 1 / data
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	 if(state == 0)
	 {
		 printf("    <LED Control>    \r\n");
		 printf("cMode = 1 ----> Red\r\n");
		 printf("cMode = 2 ----> Green\r\n");
		 printf("cMode = 3 ----> Yellow\r\n");
		 printf("cMode = 4 ----> All\r\n");
		 printf("Press cMode + [Enter]...\r\n");
		 state = 1;
	 }
	 else if (rxbuf[0] != 0) // rxbuf 에 어떤 값이라도 들어와 있다면?
	 {
		 cMode = rxbuf[0]-48; // __io_gethar() 대신 rxbuf[0]에 저장되므로
		 state = 0;
		 printf("LED Mode : %d\r\n", cMode);
		 rxbuf[0] = 0; // 다시 0을 넣어줘야 무한반복되지 않는다.
		 // cMode는 이전 rxbuf[0] 값이 들어가기 때문에 LED 출력은 유지된다.
//		 printf("\r\n");
//		 printf("\r\n");
//		 printf("\r\n");
	 }
    osDelay(200);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task_LED_R */
/**
* @brief Function implementing the Task_LED_Red thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_LED_R */
void Task_LED_R(void const * argument)
{
  /* USER CODE BEGIN Task_LED_R */
  /* Infinite loop */
  for(;;)
  {
	if(cMode == 1 || cMode == 4) HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    osDelay(100);
  }
  /* USER CODE END Task_LED_R */
}

/* USER CODE BEGIN Header_Task_LED_G */
/**
* @brief Function implementing the Task_LED_Green thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_LED_G */
void Task_LED_G(void const * argument)
{
  /* USER CODE BEGIN Task_LED_G */
  /* Infinite loop */
  for(;;)
  {
	  if(cMode == 2 || cMode == 4) HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);
    osDelay(100);
  }
  /* USER CODE END Task_LED_G */
}

/* USER CODE BEGIN Header_Task_LED_B */
/**
* @brief Function implementing the Task_LED_Blue thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_LED_B */
void Task_LED_B(void const * argument)
{
  /* USER CODE BEGIN Task_LED_B */
  /* Infinite loop */
  for(;;)
  {
	  if(cMode == 3 || cMode == 4) HAL_GPIO_TogglePin(LED_Blue_GPIO_Port, LED_Blue_Pin);
	  osDelay(100);
  }
  /* USER CODE END Task_LED_B */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
