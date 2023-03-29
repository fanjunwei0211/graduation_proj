#include "control.h"


float yaw,init_yaw;

void HWT_init()
{
	int i = 0;
	HAL_UART_Receive_IT(&huart1,&TxBuffer_T,1);
//	while(i > 20)
//	{
//		i = 0;
//		sendcmd(ACCCALSW);
//		HAL_Delay(100);//等待模块内部自动校准好，模块内部会自动计算需要一定的时间
//		sendcmd(SAVACALSW);
//		HAL_Delay(100);//保存当前配置
//		i++;
//	}
	init_yaw = (float)stcAngle.Angle[2]/32768*180;
	
}