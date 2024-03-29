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
#include "delay.h"
#include "modbus.h"
#include "nimotion.h"
#include "led.h"
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
uint8_t uart3_res;
uint8_t error =0;
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
  MX_USART3_UART_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_UART_Receive_IT(&huart3,&uart3_res,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	uint16_t temp = 0;
	
	int32_t tagvelocity = 0;
	uint8_t nAddr = 0x03;
	NiM_clearErrorState(nAddr);            //故障状态清除
	NiM_clearErrorState(nAddr+1);
	delay_ms(10);
	NiM_powerOff(nAddr);   
	NiM_powerOff(nAddr+1); 
	delay_ms(10);

	
	error = NiM_changeWorkMode(nAddr,VELOCITY_MODE);////设置为速度模式
	error = NiM_changeWorkMode(nAddr+1,VELOCITY_MODE);////设置为速度模式
	delay_ms(50);
	
	delay_ms(10);
	error = NiM_powerOn(nAddr);       
	error = NiM_powerOn(nAddr+1);     
	delay_ms(10);	//使能电机
	tagvelocity = 100;//100000/10000*60=600rpm
	error = NiM_moveVelocity(nAddr,tagvelocity);//电机以600rpm正转.
	NiM_moveVelocity(nAddr+1,tagvelocity);//电机以600rpm正转
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	tagvelocity = -100;//-600rpm
	NiM_moveVelocity(nAddr,tagvelocity);//电机以600rpm反转
	NiM_moveVelocity(nAddr+1,tagvelocity);//电机以600rpm正转
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	tagvelocity = 500;//1200rpm
	NiM_moveVelocity(nAddr,tagvelocity);//电机正1200rpm正转
	NiM_moveVelocity(nAddr+1,tagvelocity);//电机以600rpm正转

	delay_ms(1000);
	NiM_moveVelocity(nAddr,0);//电机正1200rpm正转
	NiM_powerOff(nAddr);
	NiM_moveVelocity(nAddr+1,0);//电机正1200rpm正转
	NiM_powerOff(nAddr+1);   
//	uint8_t error;
//	NiM_powerOff(nAddr);
//	temp = 0x06;
//	//601 2B 40 60 00 06 00 00 00  使电机准备
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)&temp,2);
//	if (error !=0)
//		return error;
//	temp = 0x07;
//	//601 2B 40 60 00 07 00 00 00 使电机失能
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//	if (error !=0)
//		return error;
//	temp = 0x0F;
//	//601 2B 40 60 00 0F 00 00 00 使电机使能
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//	if (error !=0)
//		return error;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		Led_Flash(2);
//		delay_ms(200);
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//1ms
	if(htim->Instance == htim6.Instance)
	{
		timer6_cnt++;
		Led_Flash(500);	
	}
	//2ms
	if(htim->Instance == htim7.Instance)
	{
		RTU_FLAG=1;
		HAL_TIM_Base_Stop_IT(&htim7);
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{    
  
 	if(huart->Instance == huart3.Instance) //接收到数据
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
	/* BEGIN:   PN:1 */	
	/* END:   PN:1 */	
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
