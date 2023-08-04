/*
 * mbrtu_master.h
 *
 *  Created on: 2022��4��28��
 *      Author: hello
 */

#ifndef MBRTU_MASTER_H_
#define MBRTU_MASTER_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


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


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// MODBUS RTU ���� API
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
/// MODBUS RTU �����������ݻص������ͳ�ʱ�ص�����
/// 
/// MBRTUMasterRecvByteISRCallback�������ڴ��ڽ����ж���
/// MBRTUMasterTimerISRCallback�������ڶ�ʱ����ʱ�ж���
///
void MBRTUMasterRecvByteISRCallback(MBRTUMaterTypeDef* psModbus, uint8_t ucByte);
void MBRTUMasterTimerISRCallback(MBRTUMaterTypeDef* psModbus);

#endif /* MBRTU_MASTER_H_ */
