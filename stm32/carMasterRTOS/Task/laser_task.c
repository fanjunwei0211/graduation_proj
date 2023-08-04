#include "laser_task.h"
#include "cmsis_os.h"
#include "laser.h"

extern osSemaphoreId LaserRecHandle;
BaseType_t aStatus;

void LaserTask(void const * argument)
{
	osDelay(500);//等待其他模块初始化完成
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2,LaserRxBuffer1,LaserDataLen);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart4,LaserRxBuffer2,LaserDataLen);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart5,LaserRxBuffer3,LaserDataLen);
//	LaserDistanceGet(4,0x0003,1);
//	LaserDistanceGet(5,0x0003,1);
//	LaserDistanceGet(6,0x0003,1);
  for(;;)
  {
    osDelay(1);
  }
}
