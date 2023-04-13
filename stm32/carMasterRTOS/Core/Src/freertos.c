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
/* USER CODE BEGIN Variables */
osThreadId MotorHandle;

/* USER CODE END Variables */
osThreadId LEDHandle;
osThreadId LaserHandle;
osThreadId IMUHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MotorTask(void const * argument);
/* USER CODE END FunctionPrototypes */

void LEDTask(void const * argument);
void LaserTask(void const * argument);
void IMUTask(void const * argument);

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
  /* definition and creation of LED */
  osThreadDef(LED, LEDTask, osPriorityNormal, 0, 128);
  LEDHandle = osThreadCreate(osThread(LED), NULL);

  /* definition and creation of Laser */
  osThreadDef(Laser, LaserTask, osPriorityIdle, 0, 128);
  LaserHandle = osThreadCreate(osThread(Laser), NULL);

  /* definition and creation of IMU */
  osThreadDef(IMU, IMUTask, osPriorityIdle, 0, 128);
  IMUHandle = osThreadCreate(osThread(IMU), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(Motor, MotorTask, osPriorityIdle, 0, 128);
  MotorHandle = osThreadCreate(osThread(Motor), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_LEDTask */
/**
  * @brief  Function implementing the LED thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_LEDTask */
void LEDTask(void const * argument)
{
  /* USER CODE BEGIN LEDTask */
  /* Infinite loop */
	
  for(;;)
  {
		Led_Flash(500);
    osDelay(1);
  }
  /* USER CODE END LEDTask */
}

/* USER CODE BEGIN Header_LaserTask */
/**
* @brief Function implementing the Laser thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LaserTask */
__weak void LaserTask(void const * argument)
{
  /* USER CODE BEGIN LaserTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LaserTask */
}

/* USER CODE BEGIN Header_IMUTask */
/**
* @brief Function implementing the IMU thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IMUTask */
__weak void IMUTask(void const * argument)
{
  /* USER CODE BEGIN IMUTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END IMUTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
__weak void MotorTask(void const * argument)
{
  for(;;)
  {
    osDelay(1);
  }
}
/* USER CODE END Application */
