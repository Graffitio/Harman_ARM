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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "I2C_lcd.h"
#include <string.h>
#include <stdlib.h>
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

/* USER CODE BEGIN PV */
uint32_t dir[10];

typedef enum{
	UP, DOWN, RIGHT, LEFT, NONE
}Direction;

typedef struct{
	int row;
	int col;
	int image_num;
	int past_position[2][16];
	uint8_t prey;
}Charactor;

typedef struct{
	int row;
	int col;
	int image_num;
	int past_position[2][16];
	uint8_t prey;
}Enemy;

typedef enum{
	ING,
	WIN,
	OVER
}_Game_status;
_Game_status game_status=ING;

typedef enum{
	LEVEL1=1,
	LEVEL2,
	LEVEL3
}_Level;
_Level level=LEVEL1;

uint8_t ClockFlag;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void JoyStick()
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 50);
	dir[0] = HAL_ADC_GetValue(&hadc1);

	HAL_ADC_PollForConversion(&hadc1, 50);
	dir[1] = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	dir[2] = !(HAL_GPIO_ReadPin(Joystick_Button_GPIO_Port, Joystick_Button_Pin));
}

Direction Dir_Joystick() // 조이스틱 방향 출력 함수
{
	if(dir[0] > 3000) return RIGHT;
	else if(dir[0] < 1000) return LEFT;
	else if(dir[1] > 3000) return UP;
	else if(dir[1] < 1000) return DOWN;
	else return NONE;
}

void Move_Pacman(Charactor *charactor, Direction direc)
{
	switch(Direction)
	{
	case RIGHT :
		charactor->col++; // RIGHT 입력 시, 1칸 이동
		if(charactor->col > 15) charactor->col = 15; // 칸 넘어가지 말고 그 자리에서 정지
		charactor->image_num &= ~(0x2);
		charactor->image_num ^= 1;
		break;

	case LEFT :
		charactor->col--;
		if(charactor->col < 0) charactor->col = 0;
		charactor->image_num |= 0x2;
		charactor->image_num ^= 1;
		break;

	case UP :
		charactor->row--;
		if(charactor->row < 0) charactor->row = 0;
		break;

	case DOWN :
		charactor->row++;
		if(charactor->row > 1) charactor->row = 1;
		break;

	default :
		break;
	}
	charactor->past_position[charactor->row][charactor->col] = 1;
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
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, dir, 2);

  // LCD 초기화 함수 호출
  lcd_init();

  // Pacman 이미지 데이터
  char pac1[] = {0x07, 0x0F, 0x1E, 0x1C, 0x1C, 0x1E, 0x0F, 0x07};
  char pac2[] = {0x07, 0x0F, 0x1E, 0x1C, 0x1E, 0x1F, 0x0F, 0x00};
  char pac3[] = {0x1C, 0x1E, 0x0F, 0x07, 0x07, 0x0F, 0x1E, 0x1C};
  char pac4[] = {0x1C, 0x1E, 0x0F, 0x07, 0x0F, 0x1F, 0x1E, 0x00};
  char enemy[] = {0x1F, 0x15, 0x1F, 0x1F, 0x0E, 0x0A, 0x0A, 0x1B};


  // Pacman 이미지를 LCD에 출력
  lcd_send_cmd(0x40); // LCD 화면의 DDRAM 주소를 설정하여 화면의 원하는 위치에 출력
  for(int i = 0 ; i < 8 ; i++)
	  lcd_send_data(pac1[i]);

  lcd_send_cmd(0x40+8);
  for(int i = 0 ; i < 8 ; i++)
	  lcd_send_data(pac2[i]);

  lcd_send_cmd(0x40+16);
  for(int i = 0 ; i < 8 ; i++)
	  lcd_send_data(pac3[i]);

  lcd_send_cmd(0x40+24);
  for(int i = 0 ; i < 8 ; i++)
	  lcd_send_data(pac4[i]);

  lcd_send_cmd(0x40+32);
  for(int i = 0 ; i < 8 ; i++)
	  lcd_send_data(enemy[i]);
  // 위 코드로 인해 pac1, pac2, pac3, pac4, enemy의 데이터가 LCD 화면에 순서대로 출력된다.
  // lcd_send_cmd()를 사용하여 DDRAM 주소를 설정
  // lcd_send_data()를 사용하여 데이터 출력하면 LCD 화면에 그래픽이 표시된다.


  // 시작 화면
  lcd_put_cur(0, 0);
  lcd_send_string("Press the Button");

  lcd_put_cur(1, 4);
  lcd_send_string("to Start");


  // 시작 버튼 누를 때까지 대기
  while(HAL_GPIO_ReadPin(Joystick_Button_GPIO_Port, Joystick_Button_Pin))
	  HAL_Delay(100); // 눌리면, 100ms 뒤 시작

  HAL_TIM_Base_Start_IT(&htim2); // TIM2를 인터럽트로 사용
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // Sound를 위한 PWM

  StartSound();

  lcd_clear();
  lcd_put_cur(0, 1);
  lcd_send_string("LEVEL 1");
  HAL_Delay(500);

  lcd_put_cur(0, 9);
  lcd_send_string("Start!");
  HAL_Delay(800);


  // Pacman Init
  Charactor pacman;
  memset(&pacman, 0, sizeof(pacman));
  pacman.prey = 31;

  // Enemy Init
  Enemy octopus;
  memset(&octopos, 0, sizeof(octopus));
  octopus.image_num = 4;
  octopus.row = 1;
  octopus.col = 8;


  lcd_clear();
  lcd_put_cur(pacman.row, pacman.col);
  lcd_send_data(pacman.image_num);


  /* USER CODE END 2 */

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