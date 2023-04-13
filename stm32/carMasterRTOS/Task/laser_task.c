#include "laser_task.h"
#include "cmsis_os.h"
#include "laser.h"



void LaserTask(void const * argument)
{
	uint8_t num=4;
	uint8_t temp;
//	HAL_UART_Receive_IT(&huart2, &temp, 1);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); //使能电机IDLE中断
	HAL_UART_Receive_DMA(&huart2,LaserRxBuffer,8);
  for(;;)
  {
//		LaserDistanceGet(num,0x0001,1);
//		LaserDistanceGet(4,0x0001,1);
		num++;
		if(num>6) num=4;
		
    osDelay(100);
  }
}
