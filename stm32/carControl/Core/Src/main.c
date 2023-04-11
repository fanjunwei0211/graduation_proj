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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "control.h"
#include "jy901.h"
#include "led.h"
#include "key.h"
#include "pid.h"
#include "delay.h"
#include "laser.h"
#include "modbus.h"
#include "nimotion.h"
#include "control.h"
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
uint8_t g_Uart2RxByte;
uint8_t uart3_res;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
	delay_init(168);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM14_Init();
  MX_TIM7_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	HWT_init();
	__HAL_TIM_CLEAR_FLAG(&htim14, TIM_FLAG_UPDATE);
	HAL_TIM_Base_Start_IT(&htim14);
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_UART_Receive_IT(&huart2, &g_Uart2RxByte, 1);
	HAL_UART_Receive_IT(&huart3,&uart3_res,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */ 
//	delay_ms(500);
//	NiM_changeWorkMode(1,VELOCITY_MODE);
//	NiM_changeWorkMode(2,VELOCITY_MODE);
//	NiM_changeWorkMode(3,VELOCITY_MODE);
//	NiM_changeWorkMode(4,VELOCITY_MODE);
//	NiM_powerOn(1);
//	NiM_powerOn(2);
//	NiM_powerOn(3);
//	NiM_powerOn(4);
//	isStraight = 0;
//	SpeedSet(200,200);
//	delay_ms(2000);
//	isStraight = 0;
//	SpeedSet(-200,-200);
//	delay_ms(2000);
//	isStraight = 1;
//	SpeedSet(200,200);
////	NiM_moveVelocity(1,200);
////	NiM_moveVelocity(2,-200);
//	delay_ms(2000);
//	isStraight = 1;
//	SpeedSet(-200,-200);
////	NiM_moveVelocity(1,-200);
////	NiM_moveVelocity(2,200);
//	delay_ms(2000);
	NiM_powerOff(1);
	NiM_powerOff(2);
	NiM_powerOff(3);
	NiM_powerOff(4);
	DirectionSet(0);
	delay_ms(3000);
	DirectionSet(1);
	delay_ms(3000);
	DirectionSet(0);
	while (1)
  {
//		LaserDistanceGet(&MBRTUHandle,0x06,0x0001,1);
//		
//		LaserDistanceGet(&MBRTUHandle,0x05,0x0001,1);
//		LaserDistanceGet(&MBRTUHandle,0x04,0x0001,1);

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
  RCC_OscInitStruct.PLL.PLLN = 168;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
	//陀螺仪串口数据接收中断
	if(huart->Instance == huart1.Instance)
	{
		CopeSerial2Data(TxBuffer_T);//陀螺仪数据解算
		HAL_UART_Receive_IT(&huart1,&TxBuffer_T,1);	
		yaw=(float)stcAngle.Angle[2]/32768*180;
	}
	//激光测距仪串口数据接收中断
	if(huart->Instance == huart2.Instance)
	{
		HAL_UART_Receive_IT(&huart2, &g_Uart2RxByte, 1);  // 激光测距仪数据解算
		LaserSerial2Data(g_Uart2RxByte);
	}
	//电机串口数据接收中断
	if(huart->Instance == huart3.Instance)
	{
		HAL_UART_Receive_IT(&huart3,&uart3_res,1); 	//读取接收到的数据
		if(RS485_RX_CNT<64)
		{
			RS485_RX_BUF[RS485_RX_CNT]=uart3_res;		//记录接收到的值
			RS485_RX_CNT++;						//接收数据增加1 
			__HAL_TIM_SET_COUNTER(&htim7,0);
			HAL_TIM_Base_Start_IT(&htim7);
		} 
		else RS485_RX_CNT=0;
		
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t num=4;
	static int delay14=0;
	//5ms定时激光测距仪T3.5时间用
	if(htim->Instance == htim14.Instance)
	{
		Led_Flash(100);
		delay14++;
		if(delay14 >5)//delay14*5ms定时
		{
			LaserDistanceGet(&MBRTUHandle,num,0x0001,1);
			num++;
			if(num>6) num=4;
			delay14 = 0;
		}
	}
	//2ms提供电机返回数据接收完毕信号
	if(htim->Instance == htim7.Instance)
	{
		RTU_FLAG=1;
		HAL_TIM_Base_Stop_IT(&htim7);
	}
}
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
