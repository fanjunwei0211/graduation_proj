#include "control_task.h"
#include "motor_task.h"
#include "cmsis_os.h"
#include "laser.h"
#include "key.h"
#include "nimotion.h"


extern float yaw,init_yaw,target_yaw;


void controlTask(void const * argument)
{
	while(KeyState(0) == 0);
//	/*******************电机速度模式运行案例***************/
//	int Motor = 3;
////	NiM_AppLocation(Motor);//应用性故障处理
////	osDelay(1000);//等待1s

//	NiM_powerOff(Motor);//脱机
//	osDelay(50);
//	NiM_powerOff(4);//脱机
//	osDelay(2000);


////	NiM_AppLocation(Motor);//应用性故障处理
////	osDelay(100);//等待1s
//	NiM_powerOn(Motor);//抱机
//	osDelay(50);
//	NiM_powerOn(4);//抱机
//	osDelay(50);
//	
////	nim_return = NiM_moveVelocity(Motor, 500); //速度100正转
////	nim_return = NiM_moveVelocity(4, 500); //速度100正转

//	for(int i=0;i<6;i++){
////		NiM_AppLocation(Motor);//应用性故障处理
////		osDelay(100);//等待1s
//		NiM_moveVelocity(Motor, 300); //速度100正转
//		osDelay(50);
//		NiM_moveVelocity(4, 300); //速度100正转
//		osDelay(600);

////		NiM_AppLocation(Motor);//应用性故障处理
////		osDelay(100);//等待10s
//		NiM_moveVelocity(Motor, -300); //速度200反转
//		osDelay(50);
//		NiM_moveVelocity(4, -300); //速度200反转
//		osDelay(600);
//	}
//	


//	NiM_powerOff(Motor);//脱机
//	NiM_powerOff(4);//脱机
//	osDelay(20);

	for(int i=0; i<5; i++){
		
		speedF = 400;
		osDelay(500);
		speedF = -400;
		osDelay(500);
	}
	ShutMotorDown();


//	DirectionSet(1);
//	osDelay(3000);
//	Forword(500,200);
//	osDelay(100);
//	
//	DirectionSet(0);
//	osDelay(3000);
//	CrossMove(600,800);
//	osDelay(100);
//	
//	DirectionSet(1);
//	osDelay(3000);
//	Forword(-500,600);
//	osDelay(100);
//	
//	DirectionSet(0);
//	osDelay(3000);
//	CrossMove(600,600);
//	osDelay(100);
//	
//	DirectionSet(1);
//	osDelay(3000);
//	Forword(800,200);
//	osDelay(100);
//	
//	speedF=0;
//	StopMotor();
  for(;;)
  {
    osDelay(1);
  }
}


void Forword(int speed,int dis)
{
	speedF = speed;
	if(dis <= laserDis[1]){
		while(laserDis[1] > dis)
		{
			osDelay(1);
		}
	}
	else{
		while(laserDis[1] < dis)
		{
			osDelay(1);
		}
	}
	
	speedF = 0;
}

void CrossMove(int speed,int dis)
{
	speedF = speed;
	if(dis <= laserDis[0]){
		while(laserDis[0] > dis)
		{
			osDelay(1);
		}
	}
	else{
		while(laserDis[0] < dis)
		{
			osDelay(1);
		}
	}
	
	speedF = 0;
}
