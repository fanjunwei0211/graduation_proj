/*
 * mbrtu_master.h
 *
 *  Created on: 2022年4月28日
 *      Author: hello
 */

#ifndef MBRTU_MASTER_H_
#define MBRTU_MASTER_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// MODBUS RTU 主机控制结构
///
///
typedef struct
{
	//
	// 收发数据缓存
	//
	uint8_t ucBuf[128];
	
	//
	// 收发数据状态
	//
	uint16_t usStatus;
	
	//
	// 如果使用了RTOS需要进行互斥,那么需要实现以下两个函数的绑定
	//
	void (*lock)(void);
	void (*unlock)(void);
	
	//
	// 微秒延时函数,用于等待超时
	//
	void (*delayms)(uint32_t nms);
	
	//
	// 定时器启动和停止函数
	//
	void (*timerStop)(void);
	void (*timerStart)(void);
	
	//
	// 发送数据函数,可以是串口、TCP等
	//
	uint32_t (*sendData)(const void* buf, uint32_t len);

}MBRTUMaterTypeDef;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// MODBUS RTU 主机 API
///
///
int MBRTUMasterReadCoils(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint8_t* pucCoilsBuffer);
int MBRTUMasterReadDiscreteInputs(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint8_t* pucDiscBuffer);
int MBRTUMasterReadHoldingRegisters(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint16_t* pusRegBuffer);
int MBRTUMasterReadInputRegisters(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint16_t* pusRegBuffer);
int MBRTUMasterWriteSingleCoil(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint8_t ucState, uint16_t usTimeout);
int MBRTUMasterWriteSingleRegister(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usRegVal, uint16_t usTimeout);
int MBRTUMasterWriteMultipleCoils(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, const uint8_t* pucStateBitsBuf, uint16_t usTimeout);
int MBRTUMasterWriteMultipleRegisters(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, const uint16_t* pusRegVal, uint16_t usTimeout);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// MODBUS RTU 主机接收数据回调函数和超时回调函数
/// 
/// MBRTUMasterRecvByteISRCallback：放置于串口接收中断中
/// MBRTUMasterTimerISRCallback：放置于定时器超时中断中
///
void MBRTUMasterRecvByteISRCallback(MBRTUMaterTypeDef* psModbus, uint8_t ucByte);
void MBRTUMasterTimerISRCallback(MBRTUMaterTypeDef* psModbus);

#endif /* MBRTU_MASTER_H_ */
