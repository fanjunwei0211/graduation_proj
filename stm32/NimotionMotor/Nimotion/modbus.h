/******************************************************************************
  *
  *   Copyright (C), 2010-2018, Beijing NiMotion Control Technology Co., Ltd.
  *
  *****************************************************************************
  *@file       : modbus.h
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
#ifndef _BSP_USART_H_
#define _BSP_USART_H_
#include "main.h"
extern uint8_t RS485_RX_BUF[64];  	
//接收到的数据长度
extern uint8_t RS485_RX_CNT;   	
//接收完整数据标志
extern volatile uint16_t RTU_FLAG;  


void USART3_Send_Buffer(uint8_t *buf,uint8_t size);
uint16_t usMBCRC16( uint8_t * pucFrame, uint16_t usLen );

#endif


