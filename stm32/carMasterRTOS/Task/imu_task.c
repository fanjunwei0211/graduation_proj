#include "imu_task.h"
#include "cmsis_os.h"
#include "usart.h"
#include "JY901.h"
#include "key.h"



float yaw,init_yaw,target_yaw;
uint8_t RxBuffer_T;




void Imu_init()
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //使能电机IDLE中断
	HAL_UART_Receive_DMA(&huart1,ImuRxBuffer,11);
//	HAL_UART_Receive_IT(&huart1,&RxBuffer_T,1);	
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	
//	//陀螺仪串口数据接收中断
//	if(huart->Instance == huart1.Instance)
//	{
//		CopeSerial2Data(RxBuffer_T);//陀螺仪数据解算
//		HAL_UART_Receive_IT(&huart1,&RxBuffer_T,1);	
//	}
//}

void IMUTask(void const * argument)
{
	Imu_init();
  for(;;)
  {
		yaw=(float)stcAngle.Angle[2]/32768*180;
		
    osDelay(2);
  }
} 	
	

