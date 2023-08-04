#include "motor_task.h"
#include "cmsis_os.h"
#include "led.h"
#include "NiMotionMotorSDK.h"
#include "NiMotionTest.h"
#include "pid.h"
#include "usart.h"



char isStraight=0;
unsigned char	MotorRxBuffer[MotorBufLen];
int speedLeft,speedRight,speedW,speedF=0;
extern float yaw,init_yaw,target_yaw;
float target_yaw_real;
pid_type_def angControl_pid;
fp32 angControl_pram[3] = {15,3,0};


//电机控制任务
void MotorTask(void const * argument)
{
	osDelay(1000);//等待外设启动
//	HAL_UARTEx_ReceiveToIdle_DMA(&huart3,MotorRxBuffer,MotorBufLen);
	
//	NiM_changeWorkMode(1,VELOCITY_MODE);osDelay	(50);
//	NiM_changeWorkMode(2,VELOCITY_MODE);osDelay	(50);
//	NiM_changeWorkMode(3,VELOCITY_MODE);osDelay	(50);
//	NiM_changeWorkMode(4,VELOCITY_MODE);osDelay	(50);
	HAL_GPIO_WritePin(SW1_GPIO_Port,SW1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(SW2_GPIO_Port,SW2_Pin,GPIO_PIN_SET);
	osDelay(50);
	PID_init(&angControl_pid,PID_DELTA,angControl_pram,2000,200);
	
	DirectionSet(1);
	uint32_t temp =0;//防止指针跑飞
	uint32_t temp1 =0;//防止指针跑飞
	CurrentPostion =&temp;//防止指针跑飞
	TAGPostion=&temp1;//防止指针跑飞
	NiM_powerOn(1);osDelay(100);
	NiM_powerOn(2);osDelay(100);
	
	
  for(;;)
  {
		target_yaw_real = AngleCorrect(target_yaw+init_yaw);
		if(target_yaw_real - yaw > 1.0)
		{
//			speedW = -200+PID_calc(&angControl_pid,yaw,target_yaw_real);
			speedW=400;
		}
		else if(target_yaw_real-yaw < -1.0)
		{
//			speedW = 200+PID_calc(&angControl_pid,yaw,target_yaw_real);
			speedW=-400;
		}	
		else 
			speedW = 0;
		SpeedSet(speedF-speedW,speedF+speedW);
    osDelay(300);
  }
}

void CarInit()
{
	init_yaw = yaw;
	
}

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
		NiM_powerOff(1);osDelay(10);
		NiM_powerOff(2);osDelay(10);
		NiM_powerOn(3);osDelay(10);
		NiM_powerOn(4);osDelay(10);
	}
	else
	{
		isStraight=1;
		HAL_GPIO_WritePin(SW1_GPIO_Port,SW1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(SW2_GPIO_Port,SW2_Pin,GPIO_PIN_RESET);
		NiM_powerOn(1);osDelay(10);
		NiM_powerOn(2);osDelay(10);
		NiM_powerOff(3);osDelay(10);
		NiM_powerOff(4);osDelay(10);
	}
	speedF = 0;
}

void ShutMotorDown(void )
{
	NiM_powerOff(1);osDelay(20);
  NiM_powerOff(2);osDelay(20);
	NiM_powerOff(3);osDelay(20);
	NiM_powerOff(4);osDelay(20);
}

void StopMotor()
{
	if(isStraight == 1){
		NiM_moveVelocity(1,0);
		osDelay(10);
		NiM_moveVelocity(2,0);
	}
	
	else{
		NiM_moveVelocity(3,0);
		osDelay(10);
		NiM_moveVelocity(4,0);
	}
	
}

void SpeedSet(int left, int right)
{
	if(isStraight == 0)
	{
		NiM_moveVelocity(4,-left);
		osDelay(5);
		NiM_moveVelocity(3,right);
	}
	else
	{
		NiM_moveVelocity(1,-left);
		osDelay(5);
		NiM_moveVelocity(2,right);
	}
}




