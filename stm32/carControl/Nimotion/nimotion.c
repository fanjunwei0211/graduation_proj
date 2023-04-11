/******************************************************************************
  *
  *   Copyright (C), 2022-2023, Beijing NiMotion Control Technology Co., Ltd.
  *
  *****************************************************************************
  *@file       : nimotion.c
  *@version    : Initial Draft
  *@author     : liuping
  *@date       : 2022/8/16
  *@brief      :
  *****************************************************************************
  **function   :
  *History     :
  *@date       : 2022/9/28
  *@author     : liupinga
  *@note       : Created file

  ****************************************************************************/
#include "nimotion.h"
#include "delay.h"
#include "modbus.h"
#include "stdint.h"
#include "string.h"
#include "tim.h"
int tr;
int a;
volatile int timer6_cnt;
// Modbus 读取数据回复
volatile int sdo_recv_cnt = 0;
extern volatile uint16_t RTU_FLAG;
//接收缓存区 	
extern uint8_t RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
//接收到的数据长度
extern uint8_t RS485_RX_CNT; 
extern void delay_ms(u16 nms);

uint8_t buffer[20] = {0};
volatile int timer6_cnt;
extern void delay_ms(u16 nms);


/********************************************************
  * @brief      : NiM_isMotorOnline
  * @details    : 判断电机是否在线
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
	                
  * @param [out]: *pOnline  返回数据     取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @retval     :0 成功，其它表示错误码
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_isMotorOnline(uint8_t nAddr, uint16_t *pOnline)
{
    if(NiM_readParam(nAddr, 0x06,(uint16_t *) pOnline,2) == 0)//判断能否读取电机数据
    {

        return 0;
    }
    else
    {
        *pOnline = 0;//电机不在线
        return 1;
    }
}

/********************************************************
  * @brief      : NiM_getMotorInfo
  * @details    : 获取电机基本信息
  * @param [in] : nAddr： 电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: pInfo： 电机信息
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_getMotorInfo(uint8_t nAddr, MOTOR_INFO* pInfo)
{
		uint8_t error;
	error = NiM_readParam(nAddr,0x55,(uint16_t *)&pInfo->szSerialNumber,4);
	if (error !=0)
		return error;
	error = NiM_readParam(nAddr,0x53,(uint16_t *)&pInfo->szSoftVersion,4);
	if (error !=0)
		return error;
	pInfo->nAddr = nAddr;
	return 0;
}

/********************************************************
  * @brief      : NiM_getErrorCode
  * @details    : 获取电机错误码
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: pErrorCode： 返回错误码 	取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_getErrorCode(uint8_t nAddr, uint16_t *pErrorCode)
{
	uint8_t error;
	error = NiM_readParam(nAddr,0x37F,(uint16_t *)pErrorCode,2);
	if (error !=0)
		return error;
	return 0;

}

/********************************************************
  * @brief      : NiM_Velocity
  * @details    : 获取电机错误码
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: pVelocity 返回速度 	取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_Velocity(uint8_t nAddr, uint32_t *pVelocity)
{
	uint8_t error;
	error = NiM_readParam(nAddr,0x3D5,(uint16_t *)pVelocity,2);
	if (error !=0)
		return error;
	return 0;

}
/********************************************************
  * @brief      : NiM_clearErrorState
  * @details    : 清除电机故障
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
 
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_clearErrorState(uint8_t nAddr)
{
	uint16_t crc16 = 0;//16位CRC校验码
	uint8_t buffer[8];
	//组合要发送的报文
	buffer[0] = nAddr;
	buffer[1] = 0x06;
	//判断寄存器的地址

	buffer[2] = 0x00;
	buffer[3] = 0x51;

	buffer[4] = 0x00;	
	buffer[5] = 0x80;
	crc16 = usMBCRC16(buffer, 6);
	buffer[6] = (uint8_t)(crc16 & 0xff);
	buffer[7] = (uint8_t)((crc16 >> 8) & 0xff);

	USART3_Send_Buffer(buffer, 8);//发送数据
	return 0;
}





/********************************************************
  * @brief      : NiM_readParam
  * @details    : 获取电机参数
  * @param [in] : nAddr：   电机地址Node-ID   取值范围：0~247   
									index：索引         取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册》第10章对象字典    
									subindex:子索引     取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册》第10章对象字典
									len：对象长度  1表示1个字节，2-2个字节，4-4个字节 取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @param [out]: *date     返回数据      取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @retval     :0 成功，其它表示错误码

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint16_t fvalv=0;
//nAddr,0x1018,0x03,(uint8_t *)&pInfo->szSoftVersion,4
uint8_t NiM_readParam(uint8_t nAddr, uint16_t index, uint16_t *data, uint8_t len)
{
	uint32_t i=0;
	uint8_t addi=0;
	uint32_t error=0;//错误状态返回值
  uint16_t crc16 = 0;//16位CRC校验码

	//组合要发送的报文
   buffer[0] = nAddr;
	 buffer[1] = 0x04;
	 //判断寄存器的地址

		buffer[2] = (index>>8)&0xff;
		buffer[3] = index&0xff;
		buffer[4] = 0x00;
    buffer[5] = len / 2;
		crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)((crc16 >> 8) & 0xff);

    USART3_Send_Buffer(buffer, 8);//发送数据
    delay_ms(1);//延时等待发送完成
	
		timer6_cnt = 0;//定时器计数清零
    while(timer6_cnt < 500)//2s超时
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
									timer6_cnt=0;//定时器计数清零
									return error;//返回故障码
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == len))//在电机非故障状态下，判断接收到的数据是否有问题
								{
									//存储电机信息数据
									for(i=0;i<(len/2);i++)
									{
										//*data = (uint32_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
										*data = (uint32_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
										 data++;
										// fvalv= (uint16_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
									}
									
									memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
									RS485_RX_CNT=0;//接收重新计数
									timer6_cnt = 0;//定时器计数清零
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
		if(timer6_cnt >=2000)//判断是否接收超时
		{
			timer6_cnt=0;//定时器计数清零
			return TIMEOUT_ERROR;//返回超时
		}
		else 
		{
			timer6_cnt = 0;//定时器计数清零
			return RETURN_SUCCESS;//返回成功
		}	

}

/********************************************************
  * @brief      : NiM_writeParam
  * @details    : 设置电机参数
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									index：索引         取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
									subindex:子索引     取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
									len：对象长度  1表示1个字节，2-2个字节，4-4个字节 取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
									date：设置值 
  * @param [out]: 无         
  * @retval     :0 成功，其它表示错误码

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
 /********************************************************/


//uint8_t NiM_writeParam(uint8_t nAddr,uint16_t index,uint8_t *data, uint8_t len)
//{
//	uint32_t i=0;
//	uint32_t error=0;//错误状态返回值
//  uint16_t crc16 = 0;//16位CRC校验码

//	//组合要发送的报文
//	buffer[0] = nAddr;

//	//判断写一个寄存器还是写多个寄存器
//	buffer[1] = 0x10;
//	//判断寄存器的地址

//	buffer[2] = (index>>8)&0xff;
//	buffer[3] = index&0xff;

//	//00 01 02    00 02 04
//	//判断寄存器数量
//	buffer[4] = 0x00;	
//	if(len>2)
//	{
//		buffer[5] = 0x02;
//	}
//	else
//	{
//		buffer[5] = 0x01;
//	}	
//	//1个字节位
//	if(len>2)
//	{
//		buffer[6] = 0x04;
//	}
//	else
//	{
//		buffer[6] = 0x02;
//	}

//	if(len==4)	
//	{
//		//数据信息
//		
//		buffer[7] = data[3];
//		buffer[8] = data[2];
//		buffer[9] = data[1];
//		buffer[10]= data[0];
//		crc16 = usMBCRC16(buffer, 11);
//		buffer[11] = (uint8_t)(crc16 & 0xff);
//		buffer[12] = (uint8_t)(crc16 >> 8);
//		USART3_Send_Buffer(buffer, 13);//发送数据
//		delay_ms(1);//延时等待发送完成
//	}
//	else
//	{
//			//数据信息
//		buffer[7] = data[1];
//		buffer[8]= data[0];
//		crc16 = usMBCRC16(buffer, 9);
//		buffer[9] = (uint8_t)(crc16 & 0xff);
//		buffer[10] = (uint8_t)(crc16 >> 8);
//		USART3_Send_Buffer(buffer, 11);//发送数据
//		delay_ms(1);//延时等待发送完成
//	}

////	/////////////////////////////////////////////
//	timer6_cnt = 0;//定时器计数清零
//	RTU_FLAG=0;//清除接收完成标志位
//	HAL_TIM_Base_Start_IT(&htim7);
//	while(timer6_cnt < 500)//2s超时
//	{
//		/* BEGIN:   PN:3 */	
//		if(RTU_FLAG == 1)//收到一帧完整数据
//		{ 
//				if(RS485_RX_BUF[0] == nAddr)//判断接收到的数据是不是正确的电机发出的
//				{
//						if(RS485_RX_BUF[1] > 0x80)//判断电机是否故障
//						{
//								switch(RS485_RX_BUF[2])//如果电机有异常，返回相应的异常状态
//								{
//									case 0x01: 
//										error= ILLEGAL_FUNCTION;
//									break;
//									case 0x02:
//										error= ILLEGAL_DATA_ADDRESS;
//									break;
//									case 0x03:
//										error= ILLEGAL_DATA_VALUE;
//									break;
//									case 0x04:
//										error= SLAVE_DEVICE_FAILURE;
//									break;
//									case 0x05:
//										error = ACKNOWLEDGE;
//									break;
//									case 0x06:
//										error = SLAVE_BUSY;
//									break;
//									case 0x0C:
//									error = MOTOR_ALARM;
//									break;	
//									default:
//									error= SLAVE_DEVICE_FAILURE;
//									break;
//								
//								}
//								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
//								RS485_RX_CNT=0;//接收重新计数
//								timer6_cnt=0;//定时器计数清零
//								return error;//返回故障码
//							/* BEGIN:   PN:3 */	
//						}
//						else
//						{
//							
//							if((RS485_RX_BUF[1] == buffer[1])&& (RS485_RX_BUF[3] == buffer[3])  && (RS485_RX_BUF[5] == buffer[5]))//在电机非故障状态下，判断接收到的数据是否有问题
//							{
//								
//								memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
//								RS485_RX_CNT=0;//接收重新计数
//								timer6_cnt = 0;//定时器计数清零
//								break;
//							}


//						}
//				}
//			memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
//			RS485_RX_CNT=0;//接收重新计数
//			RTU_FLAG=0;//清除接收完成标志位
//		}
//	}
//	memset(RS485_RX_BUF, 0, 64);//清除接收缓存区的数据
//	if(timer6_cnt >=2000)//判断是否接收超时
//	{
//		timer6_cnt=0;//定时器计数清零
//		return TIMEOUT_ERROR;//返回超时
//	}
//	else 
//	{
//		timer6_cnt = 0;//定时器计数清零
//		return RETURN_SUCCESS;//返回成功
//	}
//}

uint8_t NiM_writeParam(uint8_t nAddr,uint16_t index,uint8_t *data, uint8_t len)
{
	uint32_t i=0;
	uint32_t error=0;//错误状态返回值
  uint16_t crc16 = 0;//16位CRC校验码

	//组合要发送的报文
	buffer[0] = nAddr;

	//判断写一个寄存器还是写多个寄存器
	buffer[1] = 0x10;
	//判断寄存器的地址

	buffer[2] = (index>>8)&0xff;
	buffer[3] = index&0xff;

	//00 01 02    00 02 04
	//判断寄存器数量
	buffer[4] = 0x00;	
	if(len>2)
	{
		buffer[5] = 0x02;
	}
	else
	{
		buffer[5] = 0x01;
	}	
	//1个字节位
	if(len>2)
	{
		buffer[6] = 0x04;
	}
	else
	{
		buffer[6] = 0x02;
	}

	if(len==4)	
	{
		//数据信息
		
		buffer[7] = data[3];
		buffer[8] = data[2];
		buffer[9] = data[1];
		buffer[10]= data[0];
		crc16 = usMBCRC16(buffer, 11);
		buffer[11] = (uint8_t)(crc16 & 0xff);
		buffer[12] = (uint8_t)(crc16 >> 8);
		USART3_Send_Buffer(buffer, 13);//发送数据
//		delay_ms(1);//延时等待发送完成
	}
	else
	{
			//数据信息
		buffer[7] = data[1];
		buffer[8]= data[0];
		crc16 = usMBCRC16(buffer, 9);
		buffer[9] = (uint8_t)(crc16 & 0xff);
		buffer[10] = (uint8_t)(crc16 >> 8);
		USART3_Send_Buffer(buffer, 11);//发送数据
//		delay_ms(1);//延时等待发送完成
	}

//	/////////////////////////////////////////////
	RTU_FLAG=0;//清除接收完成标志位
	HAL_TIM_Base_Start_IT(&htim7);
	/* BEGIN:   PN:3 */	
	while(!RTU_FLAG);
	return RETURN_SUCCESS;//返回成功
}

/********************************************************
  * @brief      : NiM_isTargetReached
  * @details    : 判断位置到达
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: tr    0：未到达    1：到达
  * @retval     :0 成功，其它表示错误码
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/

uint8_t NiM_isTargetReached(uint8_t nAddr)
{
	uint32_t pStatusWord;
	uint8_t error;
	error = NiM_readParam(nAddr,0x381,(uint16_t *)&pStatusWord,2);
	tr = pStatusWord ;
	if (error!=0)
	{return error;}
	else
	{tr=(tr>>10)%2;}
		return error;
}
/********************************************************
  * @brief      : NiM_saveParams
  * @details    : 保存电机参数
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_saveParams(uint8_t nAddr)
{
	uint8_t error;
	uint32_t temp;
	temp = 0x65766173;
	error = NiM_writeParam(nAddr,0x26,(uint8_t *)&temp,4);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_restoreFactorySettings
  * @details    : 恢复电机出厂参数
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_restoreFactorySettings(uint8_t nAddr)
{
	uint8_t error;
	uint32_t temp;
	temp = 0x64616F6C; 
	error = NiM_writeParam(nAddr,0x26,(uint8_t *)&temp,4);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_changeAddr
  * @details    : 修改电机地址
  * @param [in] : nCurAddr：   电机当前地址Node-ID   取值范围： 0~247   
									nNewAddr：   电机新地址Node-ID     取值范围： 0~247   
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_changeAddr(uint8_t nCurAddr, uint8_t nNewAddr)
{
	uint8_t error;
	uint16_t temp = nNewAddr;
	error = NiM_writeParam(nCurAddr,0x230,(uint8_t *)&temp,2);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_changeWorkMode
  * @details    : 修改电机运行模式
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									nMode：   电机运行模式      取值范围：	0~6  取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_changeWorkMode(uint8_t nAddr, WORK_MODE nMode)
{
	uint8_t error;
	uint16_t temp=nMode;
	error = NiM_writeParam(nAddr,0x39,(uint8_t *)(&temp),1);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_getCurrentStatus
  * @details    : 获取电机状态字
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
	                             
  * @param [out]: pStatusWord：  返回状态字		取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @retval     :0 成功，其它表示错误码
 
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_getCurrentStatus(uint8_t nAddr, uint16_t *pStatusWord)
{
	uint8_t error;
	error = NiM_readParam(nAddr,0x381,(uint16_t *)pStatusWord,2);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_getCurrentPosition
  * @details    : 获取电机当前位置
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
	                             
  * @param [out]: pPosition：  返回当前位置		取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_getCurrentPosition(uint8_t nAddr, int32_t *pPosition)
{
	uint8_t error;
	error = NiM_readParam(nAddr,0x3C6,(uint16_t *)pPosition,4);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_moveAbsolute
  * @details    : 绝对位置运动
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 0~247   
									nType：     控制指令时序，0--立即更新，1--非立即更新 取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
	                nPosition：目标位置          取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : 数据类型：Modbus
								函数执行之前需确认两点：
											                 1，电机处于位置运动模式下
											                 2，电机使能
   *
  ********************************************************/
uint8_t NiM_moveAbsolute(uint8_t nAddr, uint8_t nType,int32_t nPosition)
{
	uint8_t temp;
	uint8_t error;
	if (nType==0)
		{
			error = NiM_writeParam(nAddr,0x3E7,(uint8_t *)(&nPosition),4);
			if (error !=0)return error;
			temp = 0x2F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			temp = 0x3F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			return 0;
		}
	else if (nType==1)
		{
			error = NiM_writeParam(nAddr,0x3E7,(uint8_t *)(&nPosition),4);
			if (error !=0)return error;
			temp = 0x0F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			temp = 0x1F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			return 0;
		}
	else
		 return WRONG_SEQUENCE ;
}

/********************************************************
  * @brief      : NiM_moveRelative
  * @details    : 相对位置运动
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 0~247   
									nType：     控制指令时序，0--立即更新，1--非立即更新 取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
	                nDistance：目标位置          取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : 数据类型：Modbus
								函数执行之前需确认两点：
											                 1，电机处于位置运动模式下
											                 2，电机使能
  *
  ********************************************************/
 uint8_t NiM_moveRelative(uint8_t nAddr, uint8_t nType,int32_t nDistance)
{
	uint8_t temp;
	uint8_t temp2;
	uint8_t error;
	if(nDistance>0)
	{
		temp2=0x00;
		error = NiM_writeParam(nAddr,0x3F3,(uint8_t *)&temp2,2);
	}
	else
	{
		temp2=0x80;
		error = NiM_writeParam(nAddr,0x3F3,(uint8_t *)&temp2,2);
	}
	
	if (nType==0)
		{
			//预设的目标位置（用户单位）
			error = NiM_writeParam(nAddr,0x3E7,(uint8_t *)(&nDistance),4);
			if (error !=0)return error;
			temp = 0x6F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			temp = 0x7F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			return 0;
		}
	else if (nType==1)
		{
			error = NiM_writeParam(nAddr,0x3E7,(uint8_t *)(&nDistance),4);
			if (error !=0)return error;
			temp = 0x4F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			temp = 0x5F;
			error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
			if (error !=0)return error;
			return 0;
		}
	else
		 return WRONG_SEQUENCE ;
}

/********************************************************
  * @brief      : NiM_moveVelocity
  * @details    : 速度模式运动
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 0~247               
                  nVelocity：目标速度          取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : 数据类型：Modbus
								函数执行之前需确认两点：
											                 1，电机处于速度运动模式下
											                 2，电机使能
  *
  ********************************************************/
//error = NiM_writeParam(nAddr,0x2017,0x01,(uint8_t *)&temp2,2);//虚拟端子使能
uint8_t NiM_moveVelocity(uint8_t nAddr, int32_t nVelocity)
{
	uint8_t error;
	static char isPower=0;
	uint16_t temp=0;
	int32_t tVelocity = nVelocity;
//	if(isPower==0){
//		isPower=1;
//		temp = 0x06;
//		//601 2B 40 60 00 06 00 00 00  使电机准备
//		NiM_writeParam(nAddr,0x51,(uint8_t *)&temp,2);
//		temp = 0x07;
//		//601 2B 40 60 00 07 00 00 00 使电机失能
//		NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//		temp = 0x0F;
//		//601 2B 40 60 00 0F 00 00 00 使电机使能
//		NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//	}
		
	if(nVelocity>0)
	{
		temp=0x01;//1->正方向
		error = NiM_writeParam(nAddr,0x52,(uint8_t *)(&temp),2);
		if(error ==0)
		{
			//速度设置
			error = NiM_writeParam(nAddr,0x55,(uint8_t *)(&nVelocity),4);
		}
	}
	else if(nVelocity<0)
	{
		temp=0x00;//0->反方向
		error = NiM_writeParam(nAddr,0x52,(uint8_t *)(&temp),2);
		if (error ==0)
		{
			//速度设置
			tVelocity=-nVelocity;
			error = NiM_writeParam(nAddr,0x55,(uint8_t *)(&tVelocity),4);
		}
	}
	return 0;
}

/********************************************************
  * @brief      : NiM_moveTorque
  * @details    : 转矩模式运动
  * @param [in] : nAddr：  	 电机地址Node-ID   取值范围： 0~247               
                  nTorque：  目标转矩          取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》第10章对象字典 
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : 数据类型：Modbus
								函数执行之前需确认两点：
											                 1，电机处于转矩运动模式下
											                 2，电机使能
  *
  ********************************************************/
uint8_t NiM_moveTorque(uint8_t nAddr, int16_t nTorque)
{ 
	uint8_t error;
//	uint8_t value1=0;
	error = NiM_writeParam(nAddr,0x3DB,(uint8_t *)(&nTorque),2);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_goHome
  * @details    : 原点回归模式运动
  * @param [in] : nAddr：  	 电机地址Node-ID       取值范围： 0~247               
                  nType：    原点回归方式          取值定义请参考《PMM60L、STM-M、BLM 系列一体化电机Modbus 通信（闭环）用户手册.pdf》 第5.5.7章功能描述
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : 数据类型：Modbus
								函数执行之前需确认两点：
											                 1，电机处于原点回归运动模式下
											                 2，电机使能
  *
  ********************************************************/
uint8_t NiM_goHome(uint8_t nAddr, int8_t nType)
{
	uint8_t error;
	uint8_t temp=7;
	error = NiM_writeParam(nAddr,0x416,(uint8_t *)(&nType),1);
	if (error !=0)
		return error;
	temp = 0x0F;
	error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	temp = 0x1F;
	error = NiM_writeParam(nAddr,0x380,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	return 0;
}
/********************************************************
  * @brief      : NiM_powerOn
  * @details    : 电机使能
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
//uint8_t NiM_powerOn(uint8_t nAddr)
//{
//	uint16_t temp=0;
//	uint8_t error;
//	temp = 0x06;
//	//601 2B 40 60 00 06 00 00 00  使电机准备
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)&temp,2);
//	if (error !=0)
//		return error;
//	temp = 0x07;
////	delay_ms(2);
//	//601 2B 40 60 00 07 00 00 00 使电机失能
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//	if (error !=0)
//		return error;
////	delay_ms(2);
//	temp = 0x0F;
//	//601 2B 40 60 00 0F 00 00 00 使电机使能
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//	if (error !=0)
//		return error;
//	return 0;
//}
uint8_t NiM_powerOn(uint8_t nAddr)
{
	uint16_t temp=0;
	uint8_t error;
	temp = 0x06;
	//601 2B 40 60 00 06 00 00 00  使电机准备
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)&temp,2);
	if (error !=0)
		return error;
	temp = 0x07;
	//601 2B 40 60 00 07 00 00 00 使电机失能
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	temp = 0x0F;
	//601 2B 40 60 00 0F 00 00 00 使电机使能
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_powerOff
  * @details    : 电机脱机
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_powerOff(uint8_t nAddr)
{
	uint16_t temp=6;
	uint8_t error;
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_stop
  * @details    : 停止当前运动
  * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_stop(uint8_t nAddr)
{
	uint16_t temp=7;
	uint8_t error;
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_fastStop
  * @details    : 急停
   * @param [in] : nAddr：   电机地址Node-ID   取值范围： 0~247   
									
  * @param [out]: 无
  * @retval     :0 成功，其它表示错误码

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : 数据类型：Modbus
  *
  ********************************************************/
uint8_t NiM_fastStop(uint8_t nAddr)
{
  uint16_t temp=2;
	uint8_t error;
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	return 0;
}



