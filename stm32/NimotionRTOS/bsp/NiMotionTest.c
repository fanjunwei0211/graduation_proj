/******************************************************************************
  *
  *   Copyright (C), 2010-2018, Beijing NiMotion Control Technology Co., Ltd.
  *
  *****************************************************************************
  *@file       : NiMotionTest.c
  *@version    : Initial Draft
  *@author     : Nimotion
  *@date       : 2019/7/12
  *@brief      :
  *****************************************************************************
  **function   :
								本程序为北京立迈胜控制技术有限公司控制MODBUS通信电机使用案例


								本程序版权为北京立迈胜控制技术有限公司所有
								未经本公司允许，不可转作其他商业用途
								如因非法转载造成一切意外事故，均与本公司无任何责任关系
  *History     :
  *@date       : 2019/7/12
  *@author     : Nimotion
  *@note       : Created file

  ****************************************************************************/
#include "NiMotionTest.h"
#include "NiMotionMotorSDK.h"
#include "string.h"
#include "cmsis_os.h"

MOTOR_INFO Information = {0}; //用来存储电机信息
uint32_t buffer1[10];//用来存储故障步骤
uint32_t buffer2[10];//用来存储电机位置
uint32_t AppFlag;
/********************************************************
  * @brief      : NiM_Test_Position_Mode
  * @details    : 电机位置模式测试
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test_Position_Mode(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;
    /*******************电机位置模式运行案例***************/
		nim_return = NiM_changeWorkMode(Motor, POSITION_MODE); //切换至位置模式
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 1;//第一步错误
			return error_step;
		}
		osDelay(100);//等待1s
		nim_return = NiM_saveAsHome(Motor); //设置原点
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 2;//第二步错误
			return error_step;
		}
		osDelay(100);//等待1s
		nim_return = NiM_powerOff(Motor); //脱机
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 3;//第三步错误
			return error_step;
		}
		osDelay(100);//等待1s
		nim_return = NiM_powerOn(Motor); //抱机
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 4;//第四步错误
			return error_step;
		}
		osDelay(100);//等待1s
		*TAGPostion=5000000;
		nim_return = NiM_moveAbsolute(Motor, 5000000); //绝对位置运行到3200
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 5;//第五步错误
			return error_step;
		}
		NiM_clearAlarms(Motor1);//清除1号电机报警
		NiM_clearErrorState(Motor1);//清除1号电机故障状态

		nim_return = NiM_moveRelative(Motor, 1000000); //相对位置正向运行1600
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 6;//第六步错误
			return error_step;
		}
		osDelay(1000);//等待10s
		nim_return = NiM_moveRelative(Motor, -1000000); //相对位置反向运行1600
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 7;//第七步错误
			return error_step;
		}
		osDelay(1000);//等待10s
		
		return 0;		
}

/********************************************************
  * @brief      : NiM_Test_Velocity_Mode
  * @details    : 电机速度模式测试
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test_Velocity_Mode(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;

    /*******************电机速度模式运行案例***************/
   
		nim_return = NiM_changeWorkMode(Motor, VELOCITY_MODE); //切换至速度模式
 		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			
			error_step = 1;//第一步错误
			return error_step;
		}
		NiM_AppLocation(Motor);//应用性故障处理
		osDelay(100);//等待1s
    nim_return = NiM_powerOff(Motor);//脱机
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			
			error_step = 2;//第二步错误
			return error_step;
		}
		NiM_AppLocation(Motor);//应用性故障处理
		osDelay(100);//等待1s
    nim_return = NiM_powerOn(Motor);//抱机
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			
			error_step = 3;//第三步错误
			return error_step;
		}
		NiM_AppLocation(Motor);//应用性故障处理
		osDelay(100);//等待1s
    nim_return = NiM_moveVelocity(Motor, 100); //速度100正转
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			
			error_step = 4;//第四步错误
			return error_step;
		} 
		NiM_AppLocation(Motor);//应用性故障处理
		osDelay(1000);//等待10s
    nim_return = NiM_moveVelocity(Motor, -200); //速度200反转
    if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 5;//第五步错误
			return error_step;
		}
		NiM_AppLocation(Motor);//应用性故障处理
		osDelay(1000);//等待10s
    nim_return = NiM_powerOff(Motor);//脱机
    if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 6;//第六步错误
			return error_step;
		}
		NiM_AppLocation(Motor);//应用性故障处理
		osDelay(100);//等待1s
		return 0;
}
/********************************************************
  * @brief      : NiM_Test_Alarm_Mode
  * @details    : 电机报警测试
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test_Alarm_Mode(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;
    /*******************电机故障报警和清除案例***************/
    nim_return = NiM_writeParam(Motor, 0x1b, 0x02, 20);//过压报警预设值设为20
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 1;//第一步错误
			return error_step;
		}
    osDelay(1000);//等待10s
    nim_return = NiM_writeParam(Motor, 0x1b, 0x02, 24);//过压报警预设值设为24
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 2;//第二步错误
			return error_step;
		}
    osDelay(1000);//等待10s
    nim_return = NiM_clearAlarms(Motor);//清除报警
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 3;//第三步错误
			return error_step;
		}
    osDelay(1000);//等待10s
    nim_return = NiM_clearErrorState(Motor);//清除故障状态
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 4;//第四步错误
			return error_step;
		}
    osDelay(1000);//等待10s
		return 0;
}

/********************************************************
  * @brief      : NiM_Test
  * @details    : 电机信息读取测试
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;	
    /*******************电机版本信息***************/
    nim_return = NiM_getMotorInfo(Motor, &Information);//获取电机版本信息
		if(nim_return != 0)//如果程序执行错误，退出此函数并返回当前步骤
		{
			error_step = 1;//第一步错误
			return error_step;
		}	
    osDelay(1000);//等待10s
		return 0;
}
/* BEGIN:   PN:4 */	
/********************************************************
  * @brief      :	NiM_AppLocation
  * @details    : 应用性故障处理
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/
void NiM_AppLocation(uint32_t Motor)
{
	uint32_t i;
	NiM_getHistoryAlarms(Motor,buffer1,&buffer2[9]);//获取历史故障报警
	if(buffer2[9]!=0)//如果存在报警
	{
		for(i=0;i<buffer2[9];i++)
		{
			switch (buffer1[i])
			{
				case OVER_CURRENT://过流
					;
				case OVER_VOLTAGE://过压
					;
				case UNDER_VOLTAGE://欠压
					;
				case TEMP_OVER://过热关机
					NiM_clearAlarms(Motor);//清除报警
					NiM_clearErrorState(Motor);//清除故障状态
				break;
				default:
					break;
			}
		}
		memset(buffer1,0,10);//清空报警值存储数组
		AppFlag=1;
	}
}
/* END:   PN:4 */	


/**** Copyright (C)2018 Nimotion All Rights Reserved **** END OF FILE ****/

