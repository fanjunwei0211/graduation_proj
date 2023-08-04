/******************************************************************************
  *
  *   Copyright (C), 2010-2018, Beijing NiMotion Control Technology Co., Ltd.
  *
  *****************************************************************************
  *@file       : NiMotionMotorSDK.c
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

#include "NiMotionMotorSDK.h"
#include "stdint.h"
#include "string.h"
#include "modbus.h"
#include "cmsis_os.h"
#include "usart.h"
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
uint8_t buffer[20] = {0};
volatile uint16_t Timer;
uint16_t TimerFlag;
uint32_t ActionFlag;//运动指令使能标志位
uint32_t MotorAddr;//需要读取当前位置的电机号
uint32_t *CurrentPostion;//存放当前位置
uint32_t *TAGPostion;//存放目标位置	
uint32_t CntFlag;
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
////=========================================================================//
////                                                                         //
////          P U B L I C   F U N C T I O N S                                //
////                                                                         //
////=========================================================================//

///***********************通信设备操作函数****************************/

void USART_Send_Buffer(uint8_t *buf,uint8_t size)
{
	HAL_UART_Transmit_DMA(&huart3,buf,size);
}


/********************************************************
  * @brief      : NiM_isMotorOnline
  * @details    : 判断电机是否在线
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

	* @param [out]: *pParamValue                返回数据：0：电机不在线，其他：电机在线
	* @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_isMotorOnline(uint32_t nAddr, uint32_t *pOnline)
{
    if(NiM_readParam(nAddr, 0x17, 0x02, pOnline) == RETURN_SUCCESS)
    {

        return RETURN_SUCCESS;
    }
    else
    {
        *pOnline = 0;
        return TIMEOUT_ERROR;
    }
}

/********************************************************
  * @brief      : NiM_getMotorInfo
  * @details    : 获取电机基本信息
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: pInfo：   电机信息
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/

uint32_t NiM_getMotorInfo(uint32_t nAddr, MOTOR_INFO* pInfo)
{
		uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x02;
    buffer[4] = 0x00;
    buffer[5] = 0X02;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 8);//发送数据

    osDelay(1);//延时等待发送完成
	
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//收到一帧完整数据
			{
				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
				{
					if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
					{
						switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
						{
							case 0x01: 
								error= ILLEGAL_FUNCTION;
							break;
							case 0x02:
								error= ILLEGAL_DATA_ADDRESS;
							break;
							case 0x03:
								error= ILLEGAL_DATA_VALUE;
							break;
							case 0x04:
								error= SLAVE_DEVICE_FAILURE;
							break;
							case 0x05:
								error = ACKNOWLEDGE;
							break;
							case 0x06:
								error = SLAVE_BUSY;
							break;
							case 0x0C:
								error = MOTOR_ALARM;
							break;							
							default:
								error= SLAVE_DEVICE_FAILURE;
							break;
						
						}
						memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
						RS485_RX_CNT=0;//接收重新计数
						Timer=0;//定时器计数清零

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X04))//在电机非故障状态下，判断接收到的数据是否有问题
						{
							//存储电机信息数据
							  pInfo->szSerialNumber[0] = RS485_RX_BUF[3];
								pInfo->szSerialNumber[1] = RS485_RX_BUF[4];
								pInfo->szSerialNumber[2] = RS485_RX_BUF[5];
								pInfo->szSerialNumber[3] = RS485_RX_BUF[6];
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer = 0;//定时器计数清零
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x04;
    buffer[4] = 0x00;
    buffer[5] = 0X02;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 8);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
				{
					if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
					{
							switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
							{
								case 0x01: 
									error= ILLEGAL_FUNCTION;
								break;
								case 0x02:
									error= ILLEGAL_DATA_ADDRESS;
								break;
								case 0x03:
									error= ILLEGAL_DATA_VALUE;
								break;
								case 0x04:
									error= SLAVE_DEVICE_FAILURE;
								break;
								case 0x05:
									error = ACKNOWLEDGE;
								break;
								case 0x06:
									error = SLAVE_BUSY;
								break;
								case 0x0C:
									error = MOTOR_ALARM;
								break;										
								default:
									error= SLAVE_DEVICE_FAILURE;
								break;
							
							}
							memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
							RS485_RX_CNT=0;//接收重新计数
							Timer=0;//定时器计数清零
							return error;//返回故障码
						
					}
					else
					{
							if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X04))//在电机非故障状态下，判断接收到的数据是否有问题
						{
							//存储电机信息数据
								pInfo->szHardVersion[0] = RS485_RX_BUF[3];
								pInfo->szHardVersion[1] = RS485_RX_BUF[4];
								pInfo->szHardVersion[2] = RS485_RX_BUF[5];
								pInfo->szHardVersion[3] = RS485_RX_BUF[6];
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer = 0;//定时器计数清零
								break;
						}
						
					}
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
		}
//    }
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
			//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x0A;
    buffer[4] = 0x00;
    buffer[5] = 0X02;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 8);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
											error = MOTOR_ALARM;
										break;										
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X04))//在电机非故障状态下，判断接收到的数据是否有问题
								{
									//存储电机信息数据
										pInfo->szSoftVersion[0] = RS485_RX_BUF[3];
										pInfo->szSoftVersion[1] = RS485_RX_BUF[4];
										pInfo->szSoftVersion[2] = RS485_RX_BUF[5];
										pInfo->szSoftVersion[3] = RS485_RX_BUF[6];
										memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
										RS485_RX_CNT=0;//接收重新计数
										Timer = 0;//定时器计数清零
										break;
								}

						}
					}
					memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
					RS485_RX_CNT=0;//接收重新计数
					RTU_FLAG=0;//清除接收完成标志位
				}
    }
		memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			Timer = 0;//定时器计数清零
			return RETURN_SUCCESS;//返回成功
		}
}

/********************************************************
  * @brief      : NiM_selfcheck
  * @details    : 执行电机自检
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: pResult： 自检结果
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_selfcheck(uint32_t nAddr, SELFCHECK_RESULT* pResult)
{
    return  NiM_writeParam(nAddr, 0x74, 2, 0x7465);
}

/********************************************************
  * @brief      : NiM_getLatestAlarm
  * @details    : 获取电机最近的报警
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: pAlarmCode： 返回报警值
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_getLatestAlarm(uint32_t nAddr, uint32_t*  pAlarmCode)
{
	return NiM_readParam(nAddr,0x1026,2,pAlarmCode);
}

/********************************************************
  * @brief      : NiM_getAlarm
  * @details    : 获取电机的报警
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247
									nNum ：   报警序号
  * @param [out]: pAlarmCode： 返回报警值
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_getAlarm(uint32_t nAddr, uint32_t*  pAlarmCode, uint8_t nNum)
{
    uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x27 + nNum;
    buffer[4] = 0x00;
    buffer[5] = 0X01;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);
     USART_Send_Buffer(buffer, 8);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
				/* BEGIN:   PN:3 */	
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
										error = MOTOR_ALARM;
										break;									
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X02))//在电机非故障状态下，判断接收到的数据是否有问题
								{
									//存储电机信息数据
										*pAlarmCode = (uint32_t)((RS485_RX_BUF[3] << 8) + (RS485_RX_BUF[4] << 0));
									
										memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
										RS485_RX_CNT=0;//接收重新计数
										Timer = 0;//定时器计数清零
										break;
								}

						}
					}
					memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
					RS485_RX_CNT=0;//接收重新计数
					RTU_FLAG=0;//清除接收完成标志位
				}
    }
		memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			Timer = 0;//定时器计数清零
			return RETURN_SUCCESS;//返回成功
		}

		Timer = 0;
    while(Timer < 2000)
    {

        if((RS485_RX_BUF[0] == nAddr) && (RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X02))
        {

            *pAlarmCode = (uint32_t)((RS485_RX_BUF[3] << 8) + (RS485_RX_BUF[4] << 0));
            memset(RS485_RX_BUF, 0, 64);

            Timer = 0;
            return 0;

        }

    }
    Timer = 0;
    return 1;
}

/********************************************************
  * @brief      : NiM_getErrorCode
  * @details    : 获取电机故障码
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: pErrorCode： 返回错误寄存器值 			 读取值请参考《一体化步进电机CAN通讯用户手册(闭环).pdf》第8.6章
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_getErrorCode(uint32_t nAddr, uint32_t* pErrorCode)
{
	return NiM_readParam(nAddr,0x1025,2,pErrorCode);
}

/********************************************************
  * @brief      : NiM_getHistoryAlarms
  * @details    : 获取电机历史报警
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: pAlarmCode： 返回报警值		读取值请参考《一体化步进电机CAN通讯用户手册(闭环).pdf》第8.6章
									pCount ：    返回报警数量	读取值请参考《一体化步进电机CAN通讯用户手册(闭环).pdf》第8.6章
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_getHistoryAlarms(uint32_t nAddr, uint32_t * pAlarmCode, uint32_t*  pCount)
{
    uint8_t i = 0, m = 0;


    for(i = 1; i < 9; i++)
    {
        NiM_getAlarm(nAddr, &pAlarmCode[m], i);
        if(pAlarmCode[m] != 0)
        {
            m++;
        }
    }
    *pCount = m;
    return 0;
}

/********************************************************
  * @brief      : NiM_clearAlarms
  * @details    : 清除电机报警
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_clearAlarms(uint32_t nAddr)
{
    return  NiM_writeParam(nAddr, 0x73, 2, 0x6c64);
}

/********************************************************
  * @brief      : NiM_clearErrorState
  * @details    : 清除电机故障
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_clearErrorState(uint32_t nAddr)
{
    return  NiM_writeParam(nAddr, 0x51, 2, 0x0080);
}
/**********************电机控制函数******************************/

/********************************************************
  * @brief      : NiM_readParam
  * @details    : 获取电机参数
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247
	                nParamID：子索引
  * @param [out]: *pParamValue
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *   @note     :
  *
  ********************************************************/
uint32_t NiM_readParam(uint32_t nAddr, uint32_t nParamID, uint32_t nBytes, uint32_t  *pParamValue)
{
		uint32_t i=0;
		uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
		//组合要发送的报文
     buffer[0] = nAddr;

    if(nParamID > 0x1000)
    {
        buffer[1] = 0x04;
    }
    else
    {
        buffer[1] = 0x03;
    }
    buffer[2] = 0x00;
    buffer[3] = nParamID & 0xff;
    buffer[4] = 0x00;
    buffer[5] = nBytes / 2;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)((crc16 >> 8) & 0xff);

    USART_Send_Buffer(buffer, 8);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
					{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
										error = MOTOR_ALARM;
										break;	
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == nBytes))//在电机非故障状态下，判断接收到的数据是否有问题
								{
									//存储电机信息数据
									for(i=0;i<(nBytes/2);i++)
									{
										*pParamValue = (uint32_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
										pParamValue++;
									}

									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer = 0;//定时器计数清零
									break;
								}


							}
					}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			Timer = 0;//定时器计数清零
			return RETURN_SUCCESS;//返回成功
		}

   

}
/********************************************************
  * @brief      : NiM_writeParam
  * @details    : 获取电机参数
  * @param [in] : nAddr：   电机地址Node-ID
	                nParamID：子索引
									nBytes 字节数
	                pParamValue
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  * @note     :
  *
  ********************************************************/
uint32_t NiM_writeParam(uint32_t nAddr, uint32_t nParamID, uint32_t nBytes, uint32_t nParamValue)
{
		uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
		//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x06;
    buffer[2] = 0x00;
    buffer[3] = nParamID & 0xff;
    buffer[4] = (uint8_t)((nParamValue >> 8) & 0xff);
    buffer[5] = (uint8_t)(nParamValue & 0xff);

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);
		osDelay(1);//延时等待发送完成
    USART_Send_Buffer(buffer, 8);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
					{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
										error = MOTOR_ALARM;
										break;	
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								/* BEGIN:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == nParamID) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
								{
									

									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer = 0;//定时器计数清零
									break;
								}


							}
					}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			Timer = 0;//定时器计数清零
			return RETURN_SUCCESS;//返回成功
		}

}

/********************************************************
  * @brief      : NiM_saveParams
  * @details    : 保存电机参数
  * @param [in] : nAddr：   电机地址Node-ID

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_saveParams(uint32_t nAddr)
{
	return NiM_writeParam(nAddr,0x08,2,0x7376);
}

/********************************************************
  * @brief      : NiM_restoreFactorySettings
  * @details    : 恢复电机出厂设置
  * @param [in] : nAddr：   电机地址Node-ID

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_restoreFactorySettings(uint32_t nAddr)
{
		return NiM_writeParam(nAddr,0x0B,2,0x6c64);

}

/********************************************************
  * @brief      : NiM_changeAddr
  * @details    : 修改电机地址
  * @param [in] : nCurAddr：   电机当前地址Node-ID
									nNewAddr：   电机新地址Node-ID
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_changeAddr(uint32_t nCurAddr, uint32_t nNewAddr)
{
		return NiM_writeParam(nCurAddr,0x00,2,nNewAddr);
}

/********************************************************
  * @brief      : NiM_changeWorkMode
  * @details    : 修改电机运行模式
  * @param [in] : nAddr：   电机地址Node-ID
									nMode：   电机运行模式
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/

uint32_t NiM_changeWorkMode(uint32_t nAddr, WORK_MODE nMode)
{
		return NiM_writeParam(nAddr,0x39,2,nMode);
}

/********************************************************
  * @brief      : NiM_getCurrentStatus
  * @details    : 获取电机状态字
  * @param [in] : nAddr：   电机地址Node-ID

  * @param [out]: pStatusWord：  返回状态字
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_getCurrentStatus(uint32_t nAddr, uint32_t *pStatusWord)
{
	  return NiM_readParam(nAddr,0x101F,2,pStatusWord);

}

/********************************************************
  * @brief      : NiM_getCurrentPosition
  * @details    : 获取电机当前位置
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: pPosition：  返回当前位置
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_getCurrentPosition(uint32_t nAddr, uint32_t* pPosition)
{
		return NiM_readParam(nAddr,0x1021,4,pPosition);
}

/********************************************************
  * @brief      : NiM_saveAsHome
  * @details    : 将电机当前位置保存为原点
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_saveAsHome(uint32_t  nAddr)
{
		 if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}		
	  return NiM_writeParam(nAddr,0x0048,2,0x5348);

}



/********************************************************
  * @brief      : NiM_saveAsZero
  * @details    : 将电机当前位置保存为零点
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~247

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_saveAsZero(uint32_t  nAddr)
{
		if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}	
		return NiM_writeParam(nAddr,0x47,2,0x535A);
}

/********************************************************
  * @brief      : NiM_moveRelative
  * @details    : 相对位置运动
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 1~247
	                nDistance：运动距离
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_moveRelative(uint32_t nAddr, int32_t nDistance)
{
		uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
		 if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}		
	//组合要发送的报文
		buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x52;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x02;
    buffer[7] = 0x00;
    if(nDistance > 0)
    {
        buffer[8] = 0x01;
    }
    else
    {
        buffer[8] = 0x00;
        nDistance = 0 - nDistance;
    }
    crc16 = usMBCRC16(buffer, 9);
    buffer[9] = (uint8_t)(crc16 & 0xff);
    buffer[10] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 11);//发送数据

    osDelay(1);//延时等待发送完成
	
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//收到一帧完整数据
			{
				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
				{
					if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
					{
						switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
						{
							case 0x01: 
								error= ILLEGAL_FUNCTION;
							break;
							case 0x02:
								error= ILLEGAL_DATA_ADDRESS;
							break;
							case 0x03:
								error= ILLEGAL_DATA_VALUE;
							break;
							case 0x04:
								error= SLAVE_DEVICE_FAILURE;
							break;
							case 0x05:
								error = ACKNOWLEDGE;
							break;
							case 0x06:
								error = SLAVE_BUSY;
							break;
							case 0x0C:
								error = MOTOR_ALARM;
							break;								
							default:
								error= SLAVE_DEVICE_FAILURE;
							break;
						
						}
						memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
						RS485_RX_CNT=0;//接收重新计数
						Timer=0;//定时器计数清零

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
						{
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer = 0;//定时器计数清零
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x53;
    buffer[4] = 0x00;
    buffer[5] = 0x02;
    buffer[6] = 0x04;
    buffer[7] = (nDistance >> 24) & 0xff;
    buffer[8] = (nDistance >> 16) & 0xff;
    buffer[9] = (nDistance >> 8) & 0xff;
    buffer[10] = (nDistance >> 0) & 0xff;
    crc16 = usMBCRC16(buffer, 11);
    buffer[11] = (uint8_t)(crc16 & 0xff);
    buffer[12] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 13);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
											error = MOTOR_ALARM;
										break;	
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								
							}
							else
							{
									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
									{
											memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
											RS485_RX_CNT=0;//接收重新计数
											Timer = 0;//定时器计数清零
											break;
									}
						
					}
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
		}

		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x51;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x02;
    buffer[7] = 0x00;
    buffer[8] = 0x4f;

    crc16 = usMBCRC16(buffer, 9);
    buffer[9] = (uint8_t)(crc16 & 0xff);
    buffer[10] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 11);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
											error = MOTOR_ALARM;
										break;											
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								
							}
							else
							{
									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
									{
											memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
											RS485_RX_CNT=0;//接收重新计数
											Timer = 0;//定时器计数清零
											break;
									}
						
					}
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
		}
//    }
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}

		//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x51;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x02;
    buffer[7] = 0x00;
    buffer[8] = 0x5f;

    crc16 = usMBCRC16(buffer, 9);
    buffer[9] = (uint8_t)(crc16 & 0xff);
    buffer[10] = (uint8_t)(crc16 >> 8);		
    USART_Send_Buffer(buffer, 11);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
											error = MOTOR_ALARM;
										break;
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
									{
											memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
											RS485_RX_CNT=0;//接收重新计数
											Timer = 0;//定时器计数清零
											break;
									}
							}
						}
					memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
					RS485_RX_CNT=0;//接收重新计数
					RTU_FLAG=0;//清除接收完成标志位
				}
    }
		memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			Timer = 0;//定时器计数清零
			return RETURN_SUCCESS;//返回成功
		}

	
	
	
   
}

/********************************************************
  * @brief      : NiM_moveAbsolute
  * @details    : 绝对位置运动
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 1~127
	                nPosition：目标位置          
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_moveAbsolute(uint32_t nAddr, int32_t nPosition)
{
		uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
		 if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}		
	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x53;
    buffer[4] = 0x00;
    buffer[5] = 0x02;
    buffer[6] = 0x04;
    buffer[7] = (nPosition >> 24) & 0xff;
    buffer[8] = (nPosition >> 16) & 0xff;
    buffer[9] = (nPosition >> 8) & 0xff;
    buffer[10] = (nPosition >> 0) & 0xff;
    crc16 = usMBCRC16(buffer, 11);
    buffer[11] = (uint8_t)(crc16 & 0xff);
    buffer[12] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 13);//发送数据

    osDelay(1);//延时等待发送完成
	
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//收到一帧完整数据
			{
				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
				{
					if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
					{
						switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
						{
							case 0x01: 
								error= ILLEGAL_FUNCTION;
							break;
							case 0x02:
								error= ILLEGAL_DATA_ADDRESS;
							break;
							case 0x03:
								error= ILLEGAL_DATA_VALUE;
							break;
							case 0x04:
								error= SLAVE_DEVICE_FAILURE;
							break;
							case 0x05:
								error = ACKNOWLEDGE;
							break;
							case 0x06:
								error = SLAVE_BUSY;
							break;
							case 0x0C:
								error = MOTOR_ALARM;
							break;							
							default:
								error= SLAVE_DEVICE_FAILURE;
							break;
						
						}
						memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
						RS485_RX_CNT=0;//接收重新计数
						Timer=0;//定时器计数清零

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
						{
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer = 0;//定时器计数清零
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x51;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x02;
    buffer[7] = 0x00;
    buffer[8] = 0x0f;

    crc16 = usMBCRC16(buffer, 9);
    buffer[9] = (uint8_t)(crc16 & 0xff);
    buffer[10] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 11);//发送数据

    osDelay(1);//延时等待发送完成
	
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG==1)//收到一帧完整数据
			{
				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
				{
					if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
					{
						switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
						{
							case 0x01: 
								error= ILLEGAL_FUNCTION;
							break;
							case 0x02:
								error= ILLEGAL_DATA_ADDRESS;
							break;
							case 0x03:
								error= ILLEGAL_DATA_VALUE;
							break;
							case 0x04:
								error= SLAVE_DEVICE_FAILURE;
							break;
							case 0x05:
								error = ACKNOWLEDGE;
							break;
							case 0x06:
								error = SLAVE_BUSY;
							break;
							case 0x0C:
								error = MOTOR_ALARM;
							break;							
							default:
								error= SLAVE_DEVICE_FAILURE;
							break;
						
						}
						memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
						RS485_RX_CNT=0;//接收重新计数
						Timer=0;//定时器计数清零

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
						{
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer = 0;//定时器计数清零
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}

		//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x51;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x02;
    buffer[7] = 0x00;
    buffer[8] = 0x1f;

    crc16 = usMBCRC16(buffer, 9);
    buffer[9] = (uint8_t)(crc16 & 0xff);
    buffer[10] = (uint8_t)(crc16 >> 8);
    USART_Send_Buffer(buffer, 11);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										case 0x0C:
											error = MOTOR_ALARM;
										break;										
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
									{
											memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
											RS485_RX_CNT=0;//接收重新计数
											Timer = 0;//定时器计数清零
											break;
									}
							}
						}
					memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
					RS485_RX_CNT=0;//接收重新计数
					RTU_FLAG=0;//清除接收完成标志位
				}
    }
		memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			Timer = 0;//定时器计数清零
			ActionFlag=1;
			return RETURN_SUCCESS;//返回成功
		}
}

/********************************************************
  * @brief      : NiM_moveVelocity
  * @details    : 速度模式运动
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 1~127
	                nVelocity：目标速度          
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_moveVelocity(uint32_t nAddr, int32_t nVelocity)
{
		uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
		 if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}	
	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x52;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x02;
    buffer[7] = 0x00;
    if(nVelocity > 0)
    {
        buffer[8] = 0x01;
    }
    else
    {
        buffer[8] = 0x00;
        nVelocity = 0 - nVelocity;
    }
    crc16 = usMBCRC16(buffer, 9);
    buffer[9] = (uint8_t)(crc16 & 0xff);
    buffer[10] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 11);//发送数据

    osDelay(1);//延时等待发送完成
	
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//收到一帧完整数据
			{
				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
				{
					if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
					{
						switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
						{
							case 0x01: 
								error= ILLEGAL_FUNCTION;
							break;
							case 0x02:
								error= ILLEGAL_DATA_ADDRESS;
							break;
							case 0x03:
								error= ILLEGAL_DATA_VALUE;
							break;
							case 0x04:
								error= SLAVE_DEVICE_FAILURE;
							break;
							case 0x05:
								error = ACKNOWLEDGE;
							break;
							case 0x06:
								error = SLAVE_BUSY;
							break;
							default:
								error= SLAVE_DEVICE_FAILURE;
							break;
						
						}
						memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
						RS485_RX_CNT=0;//接收重新计数
						Timer=0;//定时器计数清零

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
						{
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer = 0;//定时器计数清零
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
	
    	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x55;
    buffer[4] = 0x00;
    buffer[5] = 0x02;
    buffer[6] = 0x04;
    buffer[7] = (nVelocity >> 24) & 0xff;
    buffer[8] = (nVelocity >> 16) & 0xff;
    buffer[9] = (nVelocity >> 8) & 0xff;
    buffer[10] = (nVelocity >> 0) & 0xff;
    crc16 = usMBCRC16(buffer, 11);
    buffer[11] = (uint8_t)(crc16 & 0xff);
    buffer[12] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 13);//发送数据

    osDelay(1);//延时等待发送完成
	
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG==1)//收到一帧完整数据
			{
				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
				{
					if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
					{
						switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
						{
							case 0x01: 
								error= ILLEGAL_FUNCTION;
							break;
							case 0x02:
								error= ILLEGAL_DATA_ADDRESS;
							break;
							case 0x03:
								error= ILLEGAL_DATA_VALUE;
							break;
							case 0x04:
								error= SLAVE_DEVICE_FAILURE;
							break;
							case 0x05:
								error = ACKNOWLEDGE;
							break;
							case 0x06:
								error = SLAVE_BUSY;
							break;
							default:
								error= SLAVE_DEVICE_FAILURE;
							break;
						
						}
						memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
						RS485_RX_CNT=0;//接收重新计数
						Timer=0;//定时器计数清零

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
						{
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer = 0;//定时器计数清零
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
				RS485_RX_CNT=0;//接收重新计数
				RTU_FLAG=0;//清除接收完成标志位
			}
    }
		
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}

	//组合要发送的报文
    buffer[0] = nAddr;
    buffer[1] = 0x10;
    buffer[2] = 0x00;
    buffer[3] = 0x51;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x02;
    buffer[7] = 0x00;
    buffer[8] = 0x0f;

    crc16 = usMBCRC16(buffer, 9);
    buffer[9] = (uint8_t)(crc16 & 0xff);
    buffer[10] = (uint8_t)(crc16 >> 8);
    USART_Send_Buffer(buffer, 11);//发送数据
    osDelay(1);//延时等待发送完成
		Timer = 0;//定时器计数清零
    while(Timer < 2000)//2s超时
    {
			if(RTU_FLAG == 1)//收到一帧完整数据
			{
					if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
									switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
									{
										case 0x01: 
											error= ILLEGAL_FUNCTION;
										break;
										case 0x02:
											error= ILLEGAL_DATA_ADDRESS;
										break;
										case 0x03:
											error= ILLEGAL_DATA_VALUE;
										break;
										case 0x04:
											error= SLAVE_DEVICE_FAILURE;
										break;
										case 0x05:
											error = ACKNOWLEDGE;
										break;
										case 0x06:
											error = SLAVE_BUSY;
										break;
										default:
											error= SLAVE_DEVICE_FAILURE;
										break;
									
									}
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									Timer=0;//定时器计数清零
									return error;//返回故障码
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
									{
											memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
											RS485_RX_CNT=0;//接收重新计数
											Timer = 0;//定时器计数清零
											break;
									}
							}
						}
					memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
					RS485_RX_CNT=0;//接收重新计数
					RTU_FLAG=0;//清除接收完成标志位
				}
    }
		memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
		if(Timer >=2000)//判断是否接收超时
		{
			Timer=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			Timer = 0;//定时器计数清零
			return RETURN_SUCCESS;//返回成功
		}
 
}

/********************************************************
  * @brief      : NiM_goHome
  * @details    : 原点回归
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 1~127
	                nType：		 原点回归方式      取值请参考《一体化步进电机CAN通讯用户手册(闭环).pdf》第9章对象目录描述
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_goHome(uint32_t nAddr, uint32_t nType)
{
		uint32_t error=0;//错误状态返回值
    uint16_t crc16 = 0;//16位CRC校验码
		 if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}		
		error=NiM_writeParam(nAddr, 0x6B, 0x02, nType);
	  if(error != 0)
		{
				return error;
		}
		else
		{
					//组合要发送的报文
				buffer[0] = nAddr;
				buffer[1] = 0x10;
				buffer[2] = 0x00;
				buffer[3] = 0x51;
				buffer[4] = 0x00;
				buffer[5] = 0x01;
				buffer[6] = 0x02;
				buffer[7] = 0x00;
				buffer[8] = 0x0f;

				crc16 = usMBCRC16(buffer, 9);
				buffer[9] = (uint8_t)(crc16 & 0xff);
				buffer[10] = (uint8_t)(crc16 >> 8);

				USART_Send_Buffer(buffer, 11);//发送数据

				osDelay(1);//延时等待发送完成
			
				Timer = 0;//定时器计数清零
				while(Timer < 2000)//2s超时
				{
					/* BEGIN:   PN:3 */	
					if(RTU_FLAG==1)//收到一帧完整数据
					{
						if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
						{
							if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
							{
								switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
								{
									case 0x01: 
										error= ILLEGAL_FUNCTION;
									break;
									case 0x02:
										error= ILLEGAL_DATA_ADDRESS;
									break;
									case 0x03:
										error= ILLEGAL_DATA_VALUE;
									break;
									case 0x04:
										error= SLAVE_DEVICE_FAILURE;
									break;
									case 0x05:
										error = ACKNOWLEDGE;
									break;
									case 0x06:
										error = SLAVE_BUSY;
									break;
									case 0x0C:
										error = MOTOR_ALARM;
									break;
									default:
										error= SLAVE_DEVICE_FAILURE;
									break;
								
								}
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								Timer=0;//定时器计数清零

								return error;
							}
							else
							{
								if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
								{
										memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
										RS485_RX_CNT=0;//接收重新计数
										Timer = 0;//定时器计数清零
										break;
								}
							}
							
						}
						memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
						RS485_RX_CNT=0;//接收重新计数
						RTU_FLAG=0;//清除接收完成标志位
					}
				}
				
				if(Timer >=2000)//判断是否接收超时
				{
					Timer=0;//定时器计数清零
					return TIMEOUT_ERROR;//返回超时
				}
				
						//组合要发送的报文
					buffer[0] = nAddr;
					buffer[1] = 0x10;
					buffer[2] = 0x00;
					buffer[3] = 0x51;
					buffer[4] = 0x00;
					buffer[5] = 0x01;
					buffer[6] = 0x02;
					buffer[7] = 0x00;
					buffer[8] = 0x1f;

					crc16 = usMBCRC16(buffer, 9);
					buffer[9] = (uint8_t)(crc16 & 0xff);
					buffer[10] = (uint8_t)(crc16 >> 8);
					USART_Send_Buffer(buffer, 11);//发送数据
					osDelay(1);//延时等待发送完成
					Timer = 0;//定时器计数清零
					while(Timer < 2000)//2s超时
					{
						if(RTU_FLAG == 1)//收到一帧完整数据
						{
								if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
									{
										if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
										{
												switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
												{
													case 0x01: 
														error= ILLEGAL_FUNCTION;
													break;
													case 0x02:
														error= ILLEGAL_DATA_ADDRESS;
													break;
													case 0x03:
														error= ILLEGAL_DATA_VALUE;
													break;
													case 0x04:
														error= SLAVE_DEVICE_FAILURE;
													break;
													case 0x05:
														error = ACKNOWLEDGE;
													break;
													case 0x06:
														error = SLAVE_BUSY;
													break;
													case 0x0C:
														error = MOTOR_ALARM;
													break;
													default:
														error= SLAVE_DEVICE_FAILURE;
													break;
												
												}
												memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
												RS485_RX_CNT=0;//接收重新计数
												Timer=0;//定时器计数清零
												return error;//返回故障码
											/* END:   PN:3 */	
										}
										else
										{

												if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
												{
														memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
														RS485_RX_CNT=0;//接收重新计数
														Timer = 0;//定时器计数清零
														break;
												}
										}
									}
								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
								RS485_RX_CNT=0;//接收重新计数
								RTU_FLAG=0;//清除接收完成标志位
							}
					}
					memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
					if(Timer >=2000)//判断是否接收超时
					{
						Timer=0;//定时器计数清零
						return TIMEOUT_ERROR;//返回超时
					}
					else 
					{
						Timer = 0;//定时器计数清零
						return RETURN_SUCCESS;//返回成功
					}
		}
	
}

/********************************************************
  * @brief      : NiM_powerOn
  * @details    : 给电机驱动电路上电（抱机）
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~127

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_powerOn(uint32_t nAddr)
{
		if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}	
		return NiM_writeParam(nAddr,0x0051,2,0x0007);
}

/********************************************************
  * @brief      : NiM_powerOff
  * @details    : 给电机驱动电路断电（脱机）
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~127

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_powerOff(uint32_t nAddr)
{
		 if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}		
		return NiM_writeParam(nAddr,0x0051,2,0x0006);
}

/********************************************************
  * @brief      : NiM_stop
  * @details    : 停止当前动作
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~127

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_stop(uint32_t nAddr)
{
		 if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}		
	  return NiM_writeParam(nAddr,0X0051,2,0X007);
}

/********************************************************
  * @brief      : NiM_fastStop
  * @details    : 急停
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 1~127

  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_fastStop(uint32_t nAddr)
{
		if(ActionFlag!=0)//判断运动指令使能标志位是否生效
		{
			return ACKNOWLEDGE;//返回绝对位置运行未结束
		}	
		return NiM_writeParam(nAddr,0X0051,2,0X002);;
}


///**** Copyright (C)2018 Nimotion All Rights Reserved **** END OF FILE ****/

