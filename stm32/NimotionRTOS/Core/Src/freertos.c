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
#include "usart.h"
#include "NiMotionMotorSDK.h"
#include "NiMotionTest.h"
#include "key.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId MotorHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void MotorTask(void const * argument);

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

  /* definition and creation of Motor */
  osThreadDef(Motor, MotorTask, osPriorityIdle, 0, 256);
  MotorHandle = osThreadCreate(osThread(Motor), NULL);

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
    osDelay(1);
		
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_MotorTask */
/**
* @brief Function implementing the Motor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MotorTask */
void MotorTask(void const * argument)
{
  /* USER CODE BEGIN MotorTask */
  /* Infinite loop */
//	while(KeyState(0) == 0);
	uint32_t error_step = 0;
	uint32_t nim_return = 0;
	uint32_t temp =0;//防止指针跑飞
	uint32_t temp1 =0;//防止指针跑飞
	CurrentPostion =&temp;//防止指针跑飞
	TAGPostion=&temp1;//防止指针跑飞
	/*******************电机速度模式运行案例***************/
	int Motor = 3;
	NiM_AppLocation(Motor);//应用性故障处理
	osDelay(1000);//等待1s
	nim_return = NiM_changeWorkMode(Motor, VELOCITY_MODE); //切换至速度模式
	osDelay(20);
	nim_return = NiM_changeWorkMode(4, VELOCITY_MODE); //切换至速度模式

//	NiM_AppLocation(Motor);//应用性故障处理
//	osDelay(100);//等待1s
	nim_return = NiM_powerOff(Motor);//脱机
	osDelay(20);
//	nim_return = NiM_powerOff(4);//脱机
	osDelay(2000);


//	NiM_AppLocation(Motor);//应用性故障处理
//	osDelay(100);//等待1s
	nim_return = NiM_powerOn(Motor);//抱机
	osDelay(20);
//	nim_return = NiM_powerOn(4);//抱机
	osDelay(20);
	
//	nim_return = NiM_moveVelocity(Motor, 500); //速度100正转
//	nim_return = NiM_moveVelocity(4, 500); //速度100正转

	for(int i=0;i<15;i++){
//		NiM_AppLocation(Motor);//应用性故障处理
//		osDelay(100);//等待1s
		nim_return = NiM_moveVelocity(Motor, 500); //速度100正转
		osDelay(20);
//		nim_return = NiM_moveVelocity(4, 500); //速度100正转
		osDelay(600);

//		NiM_AppLocation(Motor);//应用性故障处理
//		osDelay(100);//等待10s
		nim_return = NiM_moveVelocity(Motor, -500); //速度200反转
		osDelay(20);
//		nim_return = NiM_moveVelocity(4, -500); //速度200反转
		osDelay(600);
	}
	

	NiM_AppLocation(Motor);//应用性故障处理
	osDelay(1000);//等待10s
	nim_return = NiM_powerOff(Motor);//脱机
//	osDelay(20);
//	nim_return = NiM_powerOff(4);//脱机


	NiM_AppLocation(Motor);//应用性故障处理
	osDelay(1000);//等待1s
	
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END MotorTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
