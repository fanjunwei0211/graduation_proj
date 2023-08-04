#ifndef __LASER_H
#define __LASER_H
#include "main.h"
#include "string.h"
#include "usart.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

extern uint16_t laserDis[3];
extern unsigned char LaserRxBuffer1[],LaserRxBuffer2[],LaserRxBuffer3[];
extern char UART2_RX_LEN;
#define LaserDataLen 15


void LaserSerial2Data(unsigned char ucData);
void laserOpenClose(uint8_t address,int mode);
void LaserSerial2DataL(unsigned char* ucData);
void LaserDistanceGet(uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum);
void LaserDistanceGetSerial(uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////
///// MODBUS RTU 主机控制结构
/////
/////
//typedef struct
//{
//	//
//	// 收发数据缓存
//	//
//	uint8_t ucBuf[128];
//	
//	//
//	// 收发数据状态
//	//
//	uint16_t usStatus;
//	
//	//
//	// 如果使用了RTOS需要进行互斥,那么需要实现以下两个函数的绑定
//	//
//	void (*lock)(void);
//	void (*unlock)(void);
//	
//	//
//	// 微秒延时函数,用于等待超时
//	//
//	void (*delayms)(uint32_t nms);
//	
//	//
//	// 定时器启动和停止函数
//	//
//	void (*timerStop)(void);
//	void (*timerStart)(void);
//	
//	//
//	// 发送数据函数,可以是串口、TCP等
//	//
//	uint32_t (*sendData)(const void* buf, uint32_t len);

//}MBRTUMaterTypeDef;

//extern MBRTUMaterTypeDef MBRTUHandle;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////
///// MODBUS RTU 主机 API
/////
/////
//



#endif
