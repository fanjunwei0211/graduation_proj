#include "imu_task.h"
#include "cmsis_os.h"
#include "usart.h"
#include "JY901.h"
#include "key.h"



float yaw,init_yaw,target_yaw;
uint8_t RxBuffer_T;




void Imu_init()
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //ʹ�ܵ��IDLE�ж�
	HAL_UART_Receive_DMA(&huart1,ImuRxBuffer,11);
//	HAL_UART_Receive_IT(&huart1,&RxBuffer_T,1);	
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	
//	//�����Ǵ������ݽ����ж�
//	if(huart->Instance == huart1.Instance)
//	{
//		CopeSerial2Data(RxBuffer_T);//���������ݽ���
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
	

