#ifndef __NIMOTION_H
#define __NIMOTION_H	 
#include "main.h"

#define SUCCESS         0       // 读取或者写入正确
#define WRONG_LENGTH   1       // 函数输入的数据长度错误
#define WRONG_RESPONSE   2     //接收到错误的回应 例如数据损坏 CRC错误等
#define EXCEPTION_RESPONSE   3 // 驱动器返回指令错误 
#define TIME_OUT             4 // 超过一段时间没有收到回复，回复包括正确的回复和错误回复
#define WRONG_SEQUENCE       5 //函数输入的控制时序错误


#define RETURN_SUCCESS 0              //接收成功
#define TIMEOUT_ERROR 7               //超时错误
#define ILLEGAL_FUNCTION 1            //非法功能码
#define ILLEGAL_DATA_ADDRESS 2        //非法数据地址
#define ILLEGAL_DATA_VALUE  3         //非法数据值
#define SLAVE_DEVICE_FAILURE 4        //从机设备故障
#define ACKNOWLEDGE 5                 //从机响应时间过长，需要主机经常询问
#define SLAVE_BUSY 6                  //从设备忙碌，主机在从设备闲状态时再发送
#define MOTOR_ALARM 8                 //电机内部有报警

#define NMT	   0x0
#define SYNC       0x1
#define TIME_STAMP 0x2
#define PDO1tx     0x3
#define PDO1rx     0x4
#define PDO2tx     0x5
#define PDO2rx     0x6
#define PDO3tx     0x7
#define PDO3rx     0x8
#define PDO4tx     0x9
#define PDO4rx     0xA
#define SDOtx      0xB
#define SDOrx      0xC
#define NODE_GUARD 0xE
#define LSS 	   0xF

extern int tr ;
extern int a ;
extern volatile int timer6_cnt;
//此处为结构体定义
typedef struct _MOTOR_INFO
{
    uint8_t nAddr;  //电机地址
    uint32_t szSerialNumber;    //电机序列号
    uint32_t szSoftVersion;     //软件版本号
} MOTOR_INFO,*P_MOTOR_INFO;



typedef enum _WORK_MODE
{
    POSITION_MODE = 1,
    VELOCITY_MODE = 2,
		TORQUE_MODE = 4,
    GOHOME_MODE = 6
} WORK_MODE;




//此处为函数声明

/**
 * @brief 判断电机是否在线
 * @param nAddr 电机地址
 * @param pOnline 指针，返回在线状态
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_isMotorOnline(uint8_t nAddr,uint16_t *pOnline);
/**
 * @brief 判断电机是否到达目标位置
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_isTargetReached(uint8_t nAddr);
/**
 * @brief 获取电机基本信息
 * @param nAddr 电机地址
 * @param pInfo 电机信息结构体指针
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_getMotorInfo(uint8_t nAddr, MOTOR_INFO* pInfo);

/**
 * @brief 获取电机故障码
 * @param nAddr 电机地址
 * @param pErrorCode 指针，返回错误寄存器
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_getErrorCode(uint8_t nAddr, uint16_t *pErrorCode);

/**
 * @brief 清除电机故障
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_clearErrorState(uint8_t nAddr);

/**
 * @brief 获取电机参数值
 * @param nAddr 电机地址
 * @param index 对象索引
	 @param len 对象长度
 * @param data 指针，返回参数值
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_readParam(uint8_t nAddr, uint16_t index, uint16_t *data, uint8_t len);

/**
 * @brief 设置电机参数值
 * @param nAddr 电机地址
 * @param index 对象索引
	 @param len 对象长度
 * @param data 指针，写入参数值
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_writeParam(uint8_t nAddr,uint16_t index, uint8_t *data, uint8_t len);

/**
 * @brief 保存电机参数
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_saveParams(uint8_t nAddr);

/**
 * @brief 恢复电机出厂设置
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_restoreFactorySettings(uint8_t nAddr);

/**
 * @brief 修改电机地址
 * @param nCurAddr 电机当前地址
 * @param nNewAddr 修改后的新地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_changeAddr(uint8_t nCurAddr, uint8_t nNewAddr);

/**
 * @brief 修改电机运行模式
 * @param nAddr 电机地址
 * @param nMode 运行模式
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_changeWorkMode(uint8_t nAddr, WORK_MODE nMode);

/**
 * @brief 获取电机状态字
 * @param nAddr 电机地址
 * @param pStatusWord 指针，返回状态字
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_getCurrentStatus(uint8_t nAddr, uint16_t *pStatusWord);

/**
 * @brief 获取电机当前位置
 * @param nAddr 电机地址
 * @param pPosition 指针，返回当前位置
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_getCurrentPosition(uint8_t nAddr, int32_t *pPosition);

/**
 * @brief 绝对位置运动
 * @param nAddr 电机地址
 * @param nType 0-立即更新/1-非立即更新
 * @param nPosition 目标位置
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_moveAbsolute(uint8_t nAddr, uint8_t nType,int32_t nPosition);

/**
 * @brief 相对位置运动
 * @param nAddr 电机地址
 * @param nType 控制指令时序 
 * @param nDistance 运动距离
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_moveRelative(uint8_t nAddr, uint8_t nType,int32_t nDistance);

/**
 * @brief 速度模式运动
 * @param nAddr 电机地址
 * @param nVelocity 目标速度
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_moveVelocity(uint8_t nAddr, int32_t nVelocity);

/**
 * @brief 转矩模式运动
 * @param nAddr 电机地址
 * @param nTorque 目标转矩
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_moveTorque(uint8_t nAddr, int16_t nTorque);

/**
 * @brief 原点回归
 * @param nAddr 电机地址
 * @param nType 原点回归方式
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_goHome(uint8_t nAddr, int8_t nType);

/**
 * @brief 电机使能
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_powerOn(uint8_t nAddr);

/**
 * @brief 电机脱机
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_powerOff(uint8_t nAddr);

/**
 * @brief 停止当前运动
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_stop(uint8_t nAddr);

/**
 * @brief 急停
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */
uint8_t NiM_fastStop(uint8_t nAddr);


 /**
 * @brief 实际速度获取
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

uint8_t NiM_Velocity(uint8_t nAddr, uint32_t *pVelocity);
 /**
 * @brief 实际速度获取
 * @param nAddr 电机地址
 * @return 0 成功，其它表示错误码
 */

//***************************************************************************************************************************************





















#endif

