/******************************************************************************
  *
  *   Copyright (C), 2010-2018, Beijing NiMotion Control Technology Co., Ltd.
  *
  *****************************************************************************
  *@file       : NiMotionMotorSDK.h
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
#ifndef __NIMOTION_MOTOR_SDK_H__
#define __NIMOTION_MOTOR_SDK_H__

#include "main.h"
 
#define BOOL int
#define FALSE 0
#define TRUE 1

#define CHAR char
#define UCHAR unsigned char
#define BYTE unsigned char
#define USHORT unsigned short
#define WORD unsigned short
#define UINT unsigned int
#define DWORD unsigned int
#define PVOID void*

	/* BEGIN:   PN:2 */	
#define RETURN_SUCCESS 0              //接收成功
#define TIMEOUT_ERROR 7               //超时错误
#define ILLEGAL_FUNCTION 1            //非法功能码
#define ILLEGAL_DATA_ADDRESS 2        //非法数据地址
#define ILLEGAL_DATA_VALUE  3         //非法数据值
#define SLAVE_DEVICE_FAILURE 4        //从机设备故障
#define ACKNOWLEDGE 5                 //从机响应时间过长，需要主机经常询问
#define SLAVE_BUSY 6                  //从设备忙碌，主机在从设备闲状态时再发送
#define MOTOR_ALARM 8                 //电机内部有报警		

		
#define  OVER_CURRENT                    0x2200          //过流保护
#define  OVER_VOLTAGE                    0x3110          //过压报警
#define  UNDER_VOLTAGE                   0x3120          //欠压报警
#define  TEMP_ALARM                      0x4310          //过热报警
#define  LIMIT_ALARM                     0x8612          //限位报警
#define  TEMP_OVER                       0xFF00		       //过热关机
#define  ERROR_WRONG_CMD                 0xFF01		       //错误命令		
#define  ERROR_NOTPERF_CMD               0xFF02		       //不能执行的命令	
#define  Negative_Limit_Switch           0xFF0E		       //负限位报警
#define  Positive_Limit_Switch           0xFF0F		       //正限位报警
#define  ERROR_SPI_MALFUNCTION           0xFF11      //SPI通讯错误
	/* END:   PN:2 */
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




//此处为结构体定义

typedef struct _MOTOR_INFO
{
    uint32_t nAddr;  //电机地址
    uint8_t szSerialNumber[20];    //电机序列号
    uint8_t szHardVersion[20];     //硬件版本号
    uint8_t szSoftVersion[20];     //软件版本号
} MOTOR_INFO,*P_MOTOR_INFO;

typedef struct _SELFCHECK_RESULT
{
    uint32_t nAddr;  //电机地址
    uint8_t nResult[4]; //自检结果
} SELFCHECK_RESULT, *P_SELFCHECK_RESULT;

typedef enum _WORK_MODE
{
    POSITION_MODE = 1,
    VELOCITY_MODE = 2,
    GOHOME_MODE = 3
} WORK_MODE;




#ifdef __cplusplus
extern "C" {
#endif

//此处为外部变量声明	

extern volatile uint16_t Timer;
extern uint16_t TimerFlag;
extern uint32_t ActionFlag;//运动指令使能标志位
extern uint32_t MotorAddr;//需要读取当前位置的电机号
extern uint32_t *CurrentPostion;//存放当前位置
extern uint32_t *TAGPostion;//存放目标位置	
	extern uint32_t CntFlag;
//此处为函数声明

/***********************通信设备操作函数****************************/

/**
 * @brief 判断电机是否在线
 * @param nAddr 电机地址
 * @param pOnline 指针，返回在线状态
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_isMotorOnline(uint32_t nAddr, uint32_t* pOnline);

/**
 * @brief 获取电机基本信息
 * @param nAddr 电机地址
 * @param pInfo 电机信息结构体指针
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_getMotorInfo(uint32_t nAddr, MOTOR_INFO* pInfo);

/**
 * @brief 执行电机自检
 * @param nAddr 电机地址
 * @param pResult 自检结果结构体指针
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_selfcheck(uint32_t nAddr, SELFCHECK_RESULT* pResult);

/**
 * @brief 获取电机最近的报警
 * @param nAddr 电机地址
 * @param pAlarmCode 返回报警值
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_getLatestAlarm(uint32_t nAddr, uint32_t* pAlarmCode);

/**
 * @brief 获取电机的报警
 * @param nAddr 电机地址
 * @param pAlarmCode 返回报警值
 * @param nNum 报警序号
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_getAlarm(uint32_t nAddr, uint32_t*  pAlarmCode, uint8_t nNum);
/**
 * @brief 获取电机故障码
 * @param nAddr 电机地址
 * @param pErrorCode 指针，返回错误寄存器
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_getErrorCode(uint32_t nAddr, uint32_t* pErrorCode);

/**
 * @brief 获取电机历史报警
 * @param nAddr 电机地址
 * @param pAlarmCode 数组指针，返回报警值列表
 * @param pCount 指针，返回报警数量
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_getHistoryAlarms(uint32_t nAddr, uint32_t * pAlarmCode, uint32_t*  pCount);

/**
 * @brief 清除电机报警
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_clearAlarms(uint32_t nAddr);

/**
 * @brief 清除电机故障
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_clearErrorState(uint32_t nAddr);



/**********************电机控制函数******************************/
/**
 * @brief 获取电机参数值
 * @param nAddr 电机地址
 * @param nParamID 参数ID
 * @param nBytes 字节数
 * @param pParamValue 指针，返回参数值
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_readParam(uint32_t nAddr, uint32_t nParamID,uint32_t nBytes, uint32_t  *pParamValue);
/**
 * @brief 设置电机参数值
 * @param nAddr 电机地址
 * @param nParamID 参数ID
 * @param nBytes 字节数
 * @param nParamValue 参数值
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_writeParam(uint32_t nAddr, uint32_t nParamID,uint32_t nBytes, uint32_t nParamValue);

/**
 * @brief 保存电机参数
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_saveParams(uint32_t nAddr);

/**
 * @brief 恢复电机出厂设置
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_restoreFactorySettings(uint32_t nAddr);

/**
 * @brief 修改电机地址
 * @param nCurAddr 电机当前地址
 * @param nNewAddr 修改后的新地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_changeAddr(uint32_t nCurAddr, uint32_t nNewAddr);

/**
 * @brief 修改电机运行模式
 * @param nAddr 电机地址
 * @param nMode 运行模式
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_changeWorkMode(uint32_t nAddr, WORK_MODE nMode);

/**
 * @brief 获取电机状态字
 * @param nAddr 电机地址
 * @param pStatusWord 指针，返回状态字
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_getCurrentStatus(uint32_t nAddr, uint32_t* pStatusWord);

/**
 * @brief 获取电机当前位置
 * @param nAddr 电机地址
 * @param pPosition 指针，返回当前位置
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_getCurrentPosition(uint32_t nAddr, uint32_t* pPosition);

/**
 * @brief 将电机当前位置保存为原点
 * @param nCurAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_saveAsHome(uint32_t  nAddr);

/**
 * @brief 将电机当前位置保存为零点
 * @param nCurAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_saveAsZero(uint32_t  nAddr);

/**
 * @brief 绝对位置运动
 * @param nCurAddr 电机地址
 * @param nPosition 目标位置
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_moveAbsolute(uint32_t nAddr, int32_t nPosition);

/**
 * @brief 相对位置运动
 * @param nCurAddr 电机地址
 * @param nDistance 运动距离
 * @return 0 成功，其它表示错误码
 */
uint32_t NiM_moveRelative(uint32_t nAddr, int32_t nDistance);


/**
 * @brief 速度模式运动
 * @param nCurAddr 电机地址
 * @param nVelocity 目标速度
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_moveVelocity(uint32_t nAddr, int32_t nVelocity);

/**
 * @brief 原点回归
 * @param nCurAddr 电机地址
 * @param nType 原点回归方式
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_goHome(uint32_t nAddr, uint32_t nType);

/**
 * @brief 给电机驱动电路上电（抱机）
 * @param nCurAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_powerOn(uint32_t nAddr);

/**
 * @brief 给电机驱动电路断电（脱机）
 * @param nCurAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_powerOff(uint32_t nAddr);

/**
 * @brief 停止当前动作
 * @param nCurAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_stop(uint32_t nAddr);

/**
 * @brief 急停
 * @param nCurAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint32_t NiM_fastStop(uint32_t nAddr);

	

#ifdef __cplusplus
}
#endif

#endif  //__NIMOTION_MOTOR_SDK_H__

/**** Copyright (C)2018 Nimotion All Rights Reserved **** END OF FILE ****/


