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
//		HAL_Delay(100);//�ȴ�ģ���ڲ��Զ�У׼�ã�ģ���ڲ����Զ�������Ҫһ����ʱ��
//		sendcmd(SAVACALSW);
//		HAL_Delay(100);//���浱ǰ����
//		i++;
//	}
	init_yaw = (float)stcAngle.Angle[2]/32768*180;
	
}