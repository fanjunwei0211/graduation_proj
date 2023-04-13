#include "motor_task.h"
#include "cmsis_os.h"
#include "led.h"
#include "nimotion.h"

char isStraight=0;
int speedLeft,speedRight;

float AngleCorrect(float ang)
{
	float angA=ang;
	if(ang > 180) angA = ang-360;
	if(ang < -180) angA = ang+360;
	return angA;
}
 //0-->横向   1-->前进
void DirectionSet(char mode)
{
	if(mode == 0)
	{
		isStraight=0;
		HAL_GPIO_WritePin(SW1_GPIO_Port,SW1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SW2_GPIO_Port,SW2_Pin,GPIO_PIN_SET);
		NiM_powerOff(1);
		NiM_powerOff(2);
		NiM_powerOn(3);
		NiM_powerOn(4);
	}
	else
	{
		isStraight=1;
		HAL_GPIO_WritePin(SW1_GPIO_Port,SW1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(SW2_GPIO_Port,SW2_Pin,GPIO_PIN_RESET);
		NiM_powerOn(1);
		NiM_powerOn(2);
		NiM_powerOff(3);
		NiM_powerOff(4);
	}
	speedLeft=0;
	speedRight=0;
}

void ShutMotorDown(void )
{
	NiM_powerOff(1);
  NiM_powerOff(2);
	NiM_powerOff(3);
	NiM_powerOff(4);
}

void SpeedSet(int left, int right)
{
//	static char moto1=0, moto2=0,moto3=0,moto4=0;
	if(isStraight == 0)
	{
//		if(moto1==1) {moto1 = 0;NiM_powerOff(1);}
//		if(moto2==1) {moto2 = 0;NiM_powerOff(2);}
//		if(moto3==0) {moto3 = 1;NiM_powerOn(3);}
//		if(moto4==0) {moto4 = 1;NiM_powerOn(4);}
		NiM_moveVelocity(4,-left);
		NiM_moveVelocity(3,right);
	}
	else
	{
//		if(moto1==0) {moto1 = 1;NiM_powerOn(1);}
//		if(moto2==0) {moto2 = 1;NiM_powerOn(2);}
//		if(moto3==1) {moto3 = 0;NiM_powerOff(3);}
//		if(moto4==1) {moto4 = 0;NiM_powerOff(4);}
		NiM_moveVelocity(1,-left);
		NiM_moveVelocity(2,right);
	}
}


//电机控制任务
void MotorTask(void const * argument)
{
	NiM_moveVelocity(1,0);
	NiM_moveVelocity(2,0);
	NiM_moveVelocity(3,0);
	NiM_moveVelocity(4,0);
//	DirectionSet(0);
//	osDelay(3000);
	DirectionSet(1);
//	osDelay(3000);
//	NiM_powerOn(1);	
//	NiM_powerOn(2);	
//	NiM_powerOn(3);
//	NiM_powerOn(4);
//	osDelay(3000);
//	NiM_powerOff(1);	
//	NiM_powerOff(2);	
//	NiM_powerOff(3);	
//	NiM_powerOff(4);
//	osDelay(3000);
	
//	while(1);
  for(;;)
  {
		SpeedSet(speedLeft,speedRight);
    osDelay(30);
  }
}

