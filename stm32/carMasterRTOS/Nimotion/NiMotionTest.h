/******************************************************************************
  *
  *   Copyright (C), 2010-2018, Beijing NiMotion Control Technology Co., Ltd.
  *
  *****************************************************************************
  *@file       : NiMotionTest.h
  *@version    : Initial Draft
  *@author     : Nimotion
  *@date       : 2019/7/12
  *@brief      :
  *****************************************************************************
  **function   :
								本程序为北京立迈胜控制技术有限公司控制MODBUS通信电机程序模板


								本程序版权为北京立迈胜控制技术有限公司所有
								未经本公司允许，不可转作其他商业用途
								如因非法转载造成一切意外事故，均与本公司无任何责任关系
  *History     :
  *@date       : 2019/7/12
  *@author     : Nimotion
  *@note       : Created file

  ****************************************************************************/
#ifndef __NIMOTIONTEST_H__
#define __NIMOTIONTEST_H__
#include "main.h"
#include "NiMotionMotorSDK.h"

/**---------------------------------------------*
  *external variables                           *
  *---------------------------------------------*/

/**---------------------------------------------*
  *constants                                    *
  *---------------------------------------------*/

/**---------------------------------------------*
  *macros                                       *
  *---------------------------------------------*/

/**---------------------------------------------*
  *project-wide global variables                *
  *---------------------------------------------*/

/**---------------------------------------------*
  *module-wide global variables                 *
  *---------------------------------------------*/

/**---------------------------------------------*
  *external routine prototypes                  *
  *---------------------------------------------*/

/**---------------------------------------------*
  *internal routine prototypes                  *
  *---------------------------------------------*/

/**---------------------------------------------*
  *routines' implementations                    *
  *---------------------------------------------*/

#define Motor1 0x01     //1号电机地址为0x01
#define Motor2 0x02     //2号电机地址为0x02
#define Motor3 0x04     //3号电机地址为0x04



/**---------------------------------------------*
  *external variables--------------------------*/

extern MOTOR_INFO Information;
/**---------------------------------------------*
  *constants                                    *
  *---------------------------------------------*/

/**---------------------------------------------*
  *macros                                       *
  *---------------------------------------------*/
//




//电机位置模式测试
uint32_t NiM_Test_Position_Mode(uint32_t Motor);
//电机速度模式测试
uint32_t NiM_Test_Velocity_Mode(uint32_t Motor);
//电机报警测试
uint32_t NiM_Test_Alarm_Mode(uint32_t Motor);




//电机信息读取测试
uint32_t NiM_Test(uint32_t Motor);
//电机故障恢复
void NiM_AppLocation(uint32_t Motor);



#endif  //__NIMOTIONTEST_H__

/**** Copyright (C)2018 Nimotion All Rights Reserved **** END OF FILE ****/


