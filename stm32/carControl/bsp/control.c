#include "control.h"

char isStraight=0;  //0-->平移  1-->前进
float yaw,init_yaw;
pid_type_def angControl_pid;
const float angControl_pmt[3] = {5, 1, 0}; 
volatile int speedLeft,speedRight;

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
	PID_init(&angControl_pid,PID_DELTA,angControl_pmt,2000,500);
}

float AngleCorrect(float ang)
{
	float angA;
	if(ang > 180) angA = ang-360;
	if(ang < -180) angA = ang+360;
	return angA;
}

void DirectionSet(char mode)
{
	if(mode == 0)
	{
		isStraight=0;
		HAL_GPIO_WritePin(SW1_GPIO_Port,SW1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(SW2_GPIO_Port,SW2_Pin,GPIO_PIN_RESET);
		NiM_moveVelocity(3,0);
		NiM_moveVelocity(4,0);
	}
	else
	{
		isStraight=1;
		HAL_GPIO_WritePin(SW1_GPIO_Port,SW1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SW2_GPIO_Port,SW2_Pin,GPIO_PIN_SET);
		NiM_moveVelocity(1,0);
		NiM_moveVelocity(2,0);
	}
	speedLeft=0;
	speedRight=0;
}

void SpeedSet(int left, int right)
{
	static char moto1=1, moto2=1,moto3=1,moto4=1;
	if(isStraight == 0)
	{
		if(moto1==1) {moto1 = 0;NiM_powerOff(1);}
		if(moto2==1) {moto2 = 0;NiM_powerOff(2);}
		if(moto3==0) {moto3 = 1;NiM_powerOn(3);}
		if(moto4==0) {moto4 = 1;NiM_powerOn(4);}
		NiM_moveVelocity(4,-left);
		NiM_moveVelocity(3,right);
	}
	else
	{
		if(moto1==0) {moto1 = 1;NiM_powerOn(1);}
		if(moto2==0) {moto2 = 1;NiM_powerOn(2);}
		if(moto3==1) {moto3 = 0;NiM_powerOff(3);}
		if(moto4==1) {moto4 = 0;NiM_powerOff(4);}
		NiM_moveVelocity(1,right);
		NiM_moveVelocity(2,-left);
	}
}