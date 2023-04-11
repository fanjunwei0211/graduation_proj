#ifndef __LASER_H
#define __LASER_H
#include "main.h"
#include "string.h"
#include "usart.h"
#include "tim.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

extern uint16_t laserDis[3];


void LaserSerial2Data(unsigned char ucData);
void laserOpenClose(uint8_t address,int mode);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// MODBUS RTU �������ƽṹ
///
///
typedef struct
{
	//
	// �շ����ݻ���
	//
	uint8_t ucBuf[128];
	
	//
	// �շ�����״̬
	//
	uint16_t usStatus;
	
	//
	// ���ʹ����RTOS��Ҫ���л���,��ô��Ҫʵ���������������İ�
	//
	void (*lock)(void);
	void (*unlock)(void);
	
	//
	// ΢����ʱ����,���ڵȴ���ʱ
	//
	void (*delayms)(uint32_t nms);
	
	//
	// ��ʱ��������ֹͣ����
	//
	void (*timerStop)(void);
	void (*timerStart)(void);
	
	//
	// �������ݺ���,�����Ǵ��ڡ�TCP��
	//
	uint32_t (*sendData)(const void* buf, uint32_t len);

}MBRTUMaterTypeDef;

extern MBRTUMaterTypeDef MBRTUHandle;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// MODBUS RTU ���� API
///
///
void LaserDistanceGet(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum);



#endif