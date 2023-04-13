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
								������Ϊ��������ʤ���Ƽ������޹�˾����MODBUSͨ�ŵ������ģ��


								�������ȨΪ��������ʤ���Ƽ������޹�˾����
								δ������˾��������ת��������ҵ��;
								����Ƿ�ת�����һ�������¹ʣ����뱾��˾���κ����ι�ϵ
  *History     :
  *@date       : 2019/7/12
  *@author     : Nimotion
  *@note       : Created file

  ****************************************************************************/
#ifndef _BSP_USART_H_
#define _BSP_USART_H_
#include "main.h"
extern uint8_t RS485_RX_BUF[64];  	
//���յ������ݳ���
extern uint8_t RS485_RX_CNT;   	
//�����������ݱ�־
extern volatile uint16_t RTU_FLAG;  


void USART3_Send_Buffer(uint8_t *buf,uint8_t size);
uint16_t usMBCRC16( uint8_t * pucFrame, uint16_t usLen );

#endif


