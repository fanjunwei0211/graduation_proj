/*
 * mbrtu_master.c
 *
 *  Created on: 2023.04.04
 *      Author: 樊俊伟
 */


#include "mbrtu_master.h"
static const uint8_t aucCRCHi[] =
{
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40
};

static const uint8_t aucCRCLo[] =
{
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
		0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
		0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
		0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
		0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
		0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
		0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
		0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
		0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
		0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
		0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
		0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
		0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
		0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
		0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
		0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
		0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
		0x41, 0x81, 0x80, 0x40
};

static uint16_t usMBCRC16( uint8_t * pucFrame, uint16_t usLen )
{
    
    uint8_t           ucCRCHi = 0xFF;
    uint8_t           ucCRCLo = 0xFF;
    int             iIndex;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( uint8_t )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( uint16_t )( ucCRCHi << 8 | ucCRCLo );
}

static int CheckCrcOK(const uint8_t* buf, uint32_t len)
{
    uint16_t chechcrc,checksum;
    chechcrc = (buf[len - 1] << 8) | buf[len - 2];
    checksum = usMBCRC16((uint8_t *)buf, len - 2);
    if(chechcrc != checksum)
    {
        return 0;
    }
    return 1;
}

static uint32_t MBRTUMasterRead(MBRTUMaterTypeDef* pMaster, uint8_t ucSlaveAddr, uint8_t ucCmd, uint16_t usStartAddr, uint16_t usNum)
{
    uint16_t crc;

    pMaster->ucBuf[0] = ucSlaveAddr;
    pMaster->ucBuf[1] = ucCmd;
    pMaster->ucBuf[2] = ((usStartAddr & 0XFF00) >> 8);
    pMaster->ucBuf[3] = (usStartAddr & 0XFF);
    pMaster->ucBuf[4] = ((usNum & 0XFF00) >> 8);
    pMaster->ucBuf[5] = (usNum & 0XFF);

    crc = usMBCRC16( ( uint8_t * )pMaster->ucBuf, 6);
    pMaster->ucBuf[6] = ( uint8_t )( crc & 0xFF );
    pMaster->ucBuf[7] = ( uint8_t )( crc >> 8 );

    return pMaster->sendData(pMaster->ucBuf, 8);
}

/**
 * 主机读取线圈状态
 * @param  ucSlaveAddress 从机地址
 * @param  usAddress      要读取的线圈起始地址
 * @param  usNum          要读取的线圈数量
 * @param  usTimeout      超时时间,单位毫秒
 * @param  pucCoilsBuffer 存储读取到的线圈状态，一个字节代表一个线圈状态，值范围：0/1
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterReadCoils(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint8_t* pucCoilsBuffer)
{
    int ret = -1;
    int delay;

    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->usStatus = 0;

    MBRTUMasterRead(psModbus, ucSlaveAddress, 0X01, usAddress, usNum);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X01 && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                uint8_t ucLoops = (usNum - 1) / 8 + 1;
                uint8_t ucState, ucBits;
                const uint8_t* it = &psModbus->ucBuf[3];
                while(ucLoops--)
                {
                    ucState = *it++;
                    ucBits  = 0;
                    while(usNum != 0 && ucBits < 8)
                    {
                        *pucCoilsBuffer++ = ucState & 0X01 ? 1 : 0;
                        ucState >>= 1;
                        usNum--;
                        ucBits++;
                    }
                    ucLoops--;
                }
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

/**
 * 主机读取离散量输入
 * @param  ucSlaveAddress 从机地址
 * @param  usAddress      要读取的离散量起始地址
 * @param  usNum          要读取的离散量数量
 * @param  usTimeout      超时时间,单位毫秒
 * @param  pucDiscBuffer  存储读取到的离散量输入状态，一个字节代表一个离散量的状态，值范围：0/1
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterReadDiscreteInputs(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint8_t* pucDiscBuffer)
{
    int ret = -1;
    int delay;

    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->usStatus = 0;

    MBRTUMasterRead(psModbus, ucSlaveAddress, 0X02, usAddress, usNum);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X02 && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                uint8_t ucLoops = (usNum - 1) / 8 + 1;
                uint8_t ucState, ucBits;
                const uint8_t* it = &psModbus->ucBuf[3];
                while(ucLoops--)
                {
                    ucState = *it++;
                    ucBits  = 0;
                    while(usNum != 0 && ucBits < 8)
                    {
                        *pucDiscBuffer++ = ucState & 0X01 ? 1 : 0;
                        ucState >>= 1;
                        usNum--;
                        ucBits++;
                    }
                    ucLoops--;
                }
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

/**
 * 主机读取保持寄存器
 * @param  ucSlaveAddress 从机地址
 * @param  usAddress      要读取的保持寄存器起始地址
 * @param  usNum          要读取的保持寄存器数量
 * @param  usTimeout      超时时间,单位毫秒
 * @param  pusRegBuffer   存储读取到的寄存器值
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterReadHoldingRegisters(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint16_t* pusRegBuffer)
{
    int ret = -1;
    int delay;

    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->usStatus = 0;

    MBRTUMasterRead(psModbus, ucSlaveAddress, 0X03, usAddress, usNum);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X03 && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                const uint8_t* p = &psModbus->ucBuf[3];
                while(usNum--)
                {
                    *pusRegBuffer++ = ((p[0] << 8 ) | p[1]);
                    p += 2;
                }
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

/**
 * 主机读取输入寄存器
 * @param  ucSlaveAddress 从机地址
 * @param  usAddress      要读取的输入寄存器起始地址
 * @param  usNum          要读取的输入寄存器数量
 * @param  usTimeout      超时时间,单位毫秒
 * @param  pusRegBuffer   存储读取到的寄存器值
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterReadInputRegisters(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, uint16_t usTimeout, uint16_t* pusRegBuffer)
{
    int ret = -1;
    int delay;

    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->usStatus = 0;

    MBRTUMasterRead(psModbus, ucSlaveAddress, 0X04, usAddress, usNum);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X04 && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                const uint8_t* p = &psModbus->ucBuf[3];
                while(usNum--)
                {
                    *pusRegBuffer++ = ((p[0] << 8 ) | p[1]);
                    p += 2;
                }
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

/**
 * 主机写单个线圈
 * @param  ucSlaveAddress 从机地址
 * @param  usAddress      线圈地址
 * @param  ucState        要设置的线圈状态，1或者0
 * @param  usTimeout      超时时间,单位毫秒
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterWriteSingleCoil(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint8_t ucState, uint16_t usTimeout)
{
    int ret = -1;
    int delay;
    uint16_t crc;

    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->ucBuf[0] = ucSlaveAddress;
    psModbus->ucBuf[1] = 0X05;
    psModbus->ucBuf[2] = usAddress & 0XFF;
    psModbus->ucBuf[3] = usAddress >> 8;
    psModbus->ucBuf[4] = ucState ? 0XFF : 0X00;
    psModbus->ucBuf[5] = 0X00;
    crc = usMBCRC16( ( uint8_t * )psModbus->ucBuf, 6);
    psModbus->ucBuf[6] = ( uint8_t )( crc & 0xFF );
    psModbus->ucBuf[7] = ( uint8_t )( crc >> 8 );

    psModbus->usStatus = 0;
    psModbus->sendData(psModbus->ucBuf, 8);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X05 && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

/**
 * 主机写单个寄存器
 * @param  ucSlaveAddress 从机地址
 * @param  usAddress      寄存器地址
 * @param  usRegVal       寄存器值
 * @param  usTimeout      超时时间,单位毫秒
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterWriteSingleRegister(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usRegVal, uint16_t usTimeout)
{
    int ret = -1;
    int delay;
    uint16_t crc;

    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->ucBuf[0] = ucSlaveAddress;
    psModbus->ucBuf[1] = 0X06;
    psModbus->ucBuf[2] = usAddress & 0XFF;
    psModbus->ucBuf[3] = usAddress >> 8;
    psModbus->ucBuf[4] = usRegVal >> 8;
    psModbus->ucBuf[5] = usRegVal & 0XFF;
    crc = usMBCRC16( ( uint8_t * )psModbus->ucBuf, 6);
    psModbus->ucBuf[6] = ( uint8_t )( crc & 0xFF );
    psModbus->ucBuf[7] = ( uint8_t )( crc >> 8 );

    psModbus->usStatus = 0;
    psModbus->sendData(psModbus->ucBuf, 8);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X06 && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

/**
 * 主机写多个线圈状态
 * @param  ucSlaveAddress  从机地址
 * @param  usAddress       线圈起始地址
 * @param  usNum           要写的线圈数量
 * @param  pucStateBitsBuf 存放线圈状态，1比特代表一个线圈状态
 * @param  usTimeout      超时时间,单位毫秒
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterWriteMultipleCoils(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, const uint8_t* pucBuf, uint16_t usTimeout)
{
	int i;
    int ret = -1;
    int delay;
    uint16_t crc;
    uint16_t usIndex = 0;
    uint8_t ucBits = 0, ucState = 0;
	
    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->ucBuf[usIndex++] = ucSlaveAddress;
    psModbus->ucBuf[usIndex++] = 0X0F;
    psModbus->ucBuf[usIndex++] = usAddress & 0XFF;
    psModbus->ucBuf[usIndex++] = usAddress >> 8;
    psModbus->ucBuf[usIndex++] = usNum >> 8;
    psModbus->ucBuf[usIndex++] = usNum & 0XFF;
    psModbus->ucBuf[usIndex++] = (usNum - 1) / 8 + 1;

	while(usNum != 0)
    {
		ucState = 0;
        ucBits  = usNum >= 8 ? 8 : usNum;
		for(i = ucBits - 1; i >= 0; i--, pucBuf++)
		{
			if(*pucBuf != 0)
			{
				ucState |= (1 << i);
			}
		}
		usNum -= ucBits;
		psModbus->ucBuf[usIndex++] = ucState;
    }

    crc = usMBCRC16( ( uint8_t * )psModbus->ucBuf, usIndex);
    psModbus->ucBuf[usIndex++] = ( uint8_t )( crc & 0xFF );
    psModbus->ucBuf[usIndex++] = ( uint8_t )( crc >> 8 );

    psModbus->usStatus = 0;
    psModbus->sendData(psModbus->ucBuf, usIndex);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X0F && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

/**
 * 主机写多个寄存器
 * @param  ucSlaveAddress 从机地址
 * @param  usAddress      要写的寄存器起始地址
 * @param  usNum          要写的寄存器数量
 * @param  pusRegVal      存放要写的寄存器值
 * @param  usTimeout      超时时间,单位毫秒
 * @return                0:成功  <0:执行失败
 */
int MBRTUMasterWriteMultipleRegisters(MBRTUMaterTypeDef* psModbus, uint8_t ucSlaveAddress, uint16_t usAddress, uint16_t usNum, const uint16_t* pusRegVal, uint16_t usTimeout)
{
    int ret = -1;
    int delay;
    uint16_t crc;
    uint16_t usIndex = 0;

    if(psModbus->lock != NULL)
    {
        psModbus->lock();
    }

    psModbus->ucBuf[usIndex++] = ucSlaveAddress;
    psModbus->ucBuf[usIndex++] = 0X10;
    psModbus->ucBuf[usIndex++] = usAddress & 0XFF;
    psModbus->ucBuf[usIndex++] = usAddress >> 8;
    psModbus->ucBuf[usIndex++] = usNum >> 8;
    psModbus->ucBuf[usIndex++] = usNum & 0XFF;
    psModbus->ucBuf[usIndex++] = usNum << 1;

    while (usNum--)
    {
        psModbus->ucBuf[usIndex++] = *pusRegVal >> 8;
        psModbus->ucBuf[usIndex++] = *pusRegVal & 0XFF;
        pusRegVal++;
    }

    crc = usMBCRC16( ( uint8_t * )psModbus->ucBuf, usIndex);
    psModbus->ucBuf[usIndex++] = ( uint8_t )( crc & 0xFF );
    psModbus->ucBuf[usIndex++] = ( uint8_t )( crc >> 8 );

    psModbus->usStatus = 0;
    psModbus->sendData(psModbus->ucBuf, usIndex);

    while(usTimeout != 0)
    {
        if(psModbus->usStatus & 0X8000)
        {
            if(psModbus->ucBuf[0] == ucSlaveAddress && psModbus->ucBuf[1] == 0X10 && CheckCrcOK(&psModbus->ucBuf[0], psModbus->usStatus & 0X7FFF))
            {
                ret = 0;
            }
            else
            {
                ret = -2;
            }
            psModbus->usStatus = 0;
            break;
        }
        delay = usTimeout > 5 ? 5 : usTimeout;
        usTimeout -= delay;
        psModbus->delayms(delay);
    }

    if(psModbus->unlock != NULL)
    {
        psModbus->unlock();
    }

    return ret;
}

void MBRTUMasterRecvByteISRCallback(MBRTUMaterTypeDef* psModbus, uint8_t ucByte)
{
    psModbus->timerStop();
    if(psModbus->usStatus < sizeof(psModbus->ucBuf))
    {
        psModbus->ucBuf[psModbus->usStatus++] = ucByte;
        psModbus->timerStart();
    }
    else
    {
        psModbus->usStatus |= 0X8000;
    }
}

void MBRTUMasterTimerISRCallback(MBRTUMaterTypeDef* psModbus)
{
    psModbus->timerStop();
    if(psModbus->usStatus != 0)
    {
        psModbus->usStatus |= 0X8000;
    }
}
