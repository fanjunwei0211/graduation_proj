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
								������Ϊ��������ʤ���Ƽ������޹�˾����MODBUSͨ�ŵ������ģ��


								�������ȨΪ��������ʤ���Ƽ������޹�˾����
								δ������˾��������ת��������ҵ��;
								����Ƿ�ת�����һ�������¹ʣ����뱾��˾���κ����ι�ϵ
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
uint32_t ActionFlag;//�˶�ָ��ʹ�ܱ�־λ
uint32_t MotorAddr;//��Ҫ��ȡ��ǰλ�õĵ����
uint32_t *CurrentPostion;//��ŵ�ǰλ��
uint32_t *TAGPostion;//���Ŀ��λ��	
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

///***********************ͨ���豸��������****************************/

void USART_Send_Buffer(uint8_t *buf,uint8_t size)
{
	HAL_UART_Transmit_DMA(&huart3,buf,size);
}


/********************************************************
  * @brief      : NiM_isMotorOnline
  * @details    : �жϵ���Ƿ�����
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

	* @param [out]: *pParamValue                �������ݣ�0����������ߣ��������������
	* @retval     :0 �ɹ���������ʾ������
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
  * @details    : ��ȡ���������Ϣ
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: pInfo��   �����Ϣ
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/

uint32_t NiM_getMotorInfo(uint32_t nAddr, MOTOR_INFO* pInfo)
{
		uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
	//���Ҫ���͵ı���
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x02;
    buffer[4] = 0x00;
    buffer[5] = 0X02;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 8);//��������

    osDelay(1);//��ʱ�ȴ��������
	
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//�յ�һ֡��������
			{
				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
				{
					if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
					{
						switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
						memset(RS485_RX_BUF, 0, 64);//������ջ�����������
						RS485_RX_CNT=0;//�������¼���
						Timer=0;//��ʱ����������

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X04))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
						{
							//�洢�����Ϣ����
							  pInfo->szSerialNumber[0] = RS485_RX_BUF[3];
								pInfo->szSerialNumber[1] = RS485_RX_BUF[4];
								pInfo->szSerialNumber[2] = RS485_RX_BUF[5];
								pInfo->szSerialNumber[3] = RS485_RX_BUF[6];
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer = 0;//��ʱ����������
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
	//���Ҫ���͵ı���
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x04;
    buffer[4] = 0x00;
    buffer[5] = 0X02;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 8);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
				{
					if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
					{
							switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
							memset(RS485_RX_BUF, 0, 64);//������ջ�����������
							RS485_RX_CNT=0;//�������¼���
							Timer=0;//��ʱ����������
							return error;//���ع�����
						
					}
					else
					{
							if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X04))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
						{
							//�洢�����Ϣ����
								pInfo->szHardVersion[0] = RS485_RX_BUF[3];
								pInfo->szHardVersion[1] = RS485_RX_BUF[4];
								pInfo->szHardVersion[2] = RS485_RX_BUF[5];
								pInfo->szHardVersion[3] = RS485_RX_BUF[6];
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer = 0;//��ʱ����������
								break;
						}
						
					}
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
		}
//    }
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
			//���Ҫ���͵ı���
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x0A;
    buffer[4] = 0x00;
    buffer[5] = 0X02;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);

    USART_Send_Buffer(buffer, 8);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X04))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
								{
									//�洢�����Ϣ����
										pInfo->szSoftVersion[0] = RS485_RX_BUF[3];
										pInfo->szSoftVersion[1] = RS485_RX_BUF[4];
										pInfo->szSoftVersion[2] = RS485_RX_BUF[5];
										pInfo->szSoftVersion[3] = RS485_RX_BUF[6];
										memset(RS485_RX_BUF, 0, 64);//������ջ�����������
										RS485_RX_CNT=0;//�������¼���
										Timer = 0;//��ʱ����������
										break;
								}

						}
					}
					memset(RS485_RX_BUF, 0, 64);//������ջ�����������
					RS485_RX_CNT=0;//�������¼���
					RTU_FLAG=0;//���������ɱ�־λ
				}
    }
		memset(RS485_RX_BUF, 0, 64);//������ջ�����������
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			Timer = 0;//��ʱ����������
			return RETURN_SUCCESS;//���سɹ�
		}
}

/********************************************************
  * @brief      : NiM_selfcheck
  * @details    : ִ�е���Լ�
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: pResult�� �Լ���
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : ��ȡ�������ı���
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: pAlarmCode�� ���ر���ֵ
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : ��ȡ����ı���
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247
									nNum ��   �������
  * @param [out]: pAlarmCode�� ���ر���ֵ
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_getAlarm(uint32_t nAddr, uint32_t*  pAlarmCode, uint8_t nNum)
{
    uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
	//���Ҫ���͵ı���
    buffer[0] = nAddr;
    buffer[1] = 0x04;
    buffer[2] = 0x00;
    buffer[3] = 0x27 + nNum;
    buffer[4] = 0x00;
    buffer[5] = 0X01;

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);
     USART_Send_Buffer(buffer, 8);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
				/* BEGIN:   PN:3 */	
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == 0X02))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
								{
									//�洢�����Ϣ����
										*pAlarmCode = (uint32_t)((RS485_RX_BUF[3] << 8) + (RS485_RX_BUF[4] << 0));
									
										memset(RS485_RX_BUF, 0, 64);//������ջ�����������
										RS485_RX_CNT=0;//�������¼���
										Timer = 0;//��ʱ����������
										break;
								}

						}
					}
					memset(RS485_RX_BUF, 0, 64);//������ջ�����������
					RS485_RX_CNT=0;//�������¼���
					RTU_FLAG=0;//���������ɱ�־λ
				}
    }
		memset(RS485_RX_BUF, 0, 64);//������ջ�����������
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			Timer = 0;//��ʱ����������
			return RETURN_SUCCESS;//���سɹ�
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
  * @details    : ��ȡ���������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: pErrorCode�� ���ش���Ĵ���ֵ 			 ��ȡֵ��ο���һ�廯�������CANͨѶ�û��ֲ�(�ջ�).pdf����8.6��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : ��ȡ�����ʷ����
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: pAlarmCode�� ���ر���ֵ		��ȡֵ��ο���һ�廯�������CANͨѶ�û��ֲ�(�ջ�).pdf����8.6��
									pCount ��    ���ر�������	��ȡֵ��ο���һ�廯�������CANͨѶ�û��ֲ�(�ջ�).pdf����8.6��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : ����������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : ����������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
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
/**********************������ƺ���******************************/

/********************************************************
  * @brief      : NiM_readParam
  * @details    : ��ȡ�������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247
	                nParamID��������
  * @param [out]: *pParamValue
  * @retval     :0 �ɹ���������ʾ������
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
		uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
		//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 8);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
					{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == nBytes))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
								{
									//�洢�����Ϣ����
									for(i=0;i<(nBytes/2);i++)
									{
										*pParamValue = (uint32_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
										pParamValue++;
									}

									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer = 0;//��ʱ����������
									break;
								}


							}
					}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		memset(RS485_RX_BUF, 0, 64);//������ջ�����������
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			Timer = 0;//��ʱ����������
			return RETURN_SUCCESS;//���سɹ�
		}

   

}
/********************************************************
  * @brief      : NiM_writeParam
  * @details    : ��ȡ�������
  * @param [in] : nAddr��   �����ַNode-ID
	                nParamID��������
									nBytes �ֽ���
	                pParamValue
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
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
		uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
		//���Ҫ���͵ı���
    buffer[0] = nAddr;
    buffer[1] = 0x06;
    buffer[2] = 0x00;
    buffer[3] = nParamID & 0xff;
    buffer[4] = (uint8_t)((nParamValue >> 8) & 0xff);
    buffer[5] = (uint8_t)(nParamValue & 0xff);

    crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)(crc16 >> 8);
		osDelay(1);//��ʱ�ȴ��������
    USART_Send_Buffer(buffer, 8);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
					{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								/* BEGIN:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == nParamID) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
								{
									

									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer = 0;//��ʱ����������
									break;
								}


							}
					}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		memset(RS485_RX_BUF, 0, 64);//������ջ�����������
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			Timer = 0;//��ʱ����������
			return RETURN_SUCCESS;//���سɹ�
		}

}

/********************************************************
  * @brief      : NiM_saveParams
  * @details    : ����������
  * @param [in] : nAddr��   �����ַNode-ID

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : �ָ������������
  * @param [in] : nAddr��   �����ַNode-ID

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : �޸ĵ����ַ
  * @param [in] : nCurAddr��   �����ǰ��ַNode-ID
									nNewAddr��   ����µ�ַNode-ID
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : �޸ĵ������ģʽ
  * @param [in] : nAddr��   �����ַNode-ID
									nMode��   �������ģʽ
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : ��ȡ���״̬��
  * @param [in] : nAddr��   �����ַNode-ID

  * @param [out]: pStatusWord��  ����״̬��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : ��ȡ�����ǰλ��
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: pPosition��  ���ص�ǰλ��
  * @retval     :0 �ɹ���������ʾ������
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
  * @details    : �������ǰλ�ñ���Ϊԭ��
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_saveAsHome(uint32_t  nAddr)
{
		 if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}		
	  return NiM_writeParam(nAddr,0x0048,2,0x5348);

}



/********************************************************
  * @brief      : NiM_saveAsZero
  * @details    : �������ǰλ�ñ���Ϊ���
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~247

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion
  *
  ********************************************************/
uint32_t NiM_saveAsZero(uint32_t  nAddr)
{
		if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}	
		return NiM_writeParam(nAddr,0x47,2,0x535A);
}

/********************************************************
  * @brief      : NiM_moveRelative
  * @details    : ���λ���˶�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 1~247
	                nDistance���˶�����
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_moveRelative(uint32_t nAddr, int32_t nDistance)
{
		uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
		 if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}		
	//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 11);//��������

    osDelay(1);//��ʱ�ȴ��������
	
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//�յ�һ֡��������
			{
				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
				{
					if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
					{
						switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
						memset(RS485_RX_BUF, 0, 64);//������ջ�����������
						RS485_RX_CNT=0;//�������¼���
						Timer=0;//��ʱ����������

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
						{
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer = 0;//��ʱ����������
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
	//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 13);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								
							}
							else
							{
									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
									{
											memset(RS485_RX_BUF, 0, 64);//������ջ�����������
											RS485_RX_CNT=0;//�������¼���
											Timer = 0;//��ʱ����������
											break;
									}
						
					}
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
		}

		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 11);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								
							}
							else
							{
									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
									{
											memset(RS485_RX_BUF, 0, 64);//������ջ�����������
											RS485_RX_CNT=0;//�������¼���
											Timer = 0;//��ʱ����������
											break;
									}
						
					}
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
		}
//    }
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}

		//���Ҫ���͵ı���
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
    USART_Send_Buffer(buffer, 11);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
									{
											memset(RS485_RX_BUF, 0, 64);//������ջ�����������
											RS485_RX_CNT=0;//�������¼���
											Timer = 0;//��ʱ����������
											break;
									}
							}
						}
					memset(RS485_RX_BUF, 0, 64);//������ջ�����������
					RS485_RX_CNT=0;//�������¼���
					RTU_FLAG=0;//���������ɱ�־λ
				}
    }
		memset(RS485_RX_BUF, 0, 64);//������ջ�����������
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			Timer = 0;//��ʱ����������
			return RETURN_SUCCESS;//���سɹ�
		}

	
	
	
   
}

/********************************************************
  * @brief      : NiM_moveAbsolute
  * @details    : ����λ���˶�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 1~127
	                nPosition��Ŀ��λ��          
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_moveAbsolute(uint32_t nAddr, int32_t nPosition)
{
		uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
		 if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}		
	//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 13);//��������

    osDelay(1);//��ʱ�ȴ��������
	
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//�յ�һ֡��������
			{
				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
				{
					if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
					{
						switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
						memset(RS485_RX_BUF, 0, 64);//������ջ�����������
						RS485_RX_CNT=0;//�������¼���
						Timer=0;//��ʱ����������

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
						{
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer = 0;//��ʱ����������
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
	//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 11);//��������

    osDelay(1);//��ʱ�ȴ��������
	
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG==1)//�յ�һ֡��������
			{
				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
				{
					if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
					{
						switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
						memset(RS485_RX_BUF, 0, 64);//������ջ�����������
						RS485_RX_CNT=0;//�������¼���
						Timer=0;//��ʱ����������

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
						{
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer = 0;//��ʱ����������
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}

		//���Ҫ���͵ı���
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
    USART_Send_Buffer(buffer, 11);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
									{
											memset(RS485_RX_BUF, 0, 64);//������ջ�����������
											RS485_RX_CNT=0;//�������¼���
											Timer = 0;//��ʱ����������
											break;
									}
							}
						}
					memset(RS485_RX_BUF, 0, 64);//������ջ�����������
					RS485_RX_CNT=0;//�������¼���
					RTU_FLAG=0;//���������ɱ�־λ
				}
    }
		memset(RS485_RX_BUF, 0, 64);//������ջ�����������
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			Timer = 0;//��ʱ����������
			ActionFlag=1;
			return RETURN_SUCCESS;//���سɹ�
		}
}

/********************************************************
  * @brief      : NiM_moveVelocity
  * @details    : �ٶ�ģʽ�˶�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 1~127
	                nVelocity��Ŀ���ٶ�          
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_moveVelocity(uint32_t nAddr, int32_t nVelocity)
{
		uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
		 if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}	
	//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 11);//��������

    osDelay(1);//��ʱ�ȴ��������
	
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			/* BEGIN:   PN:3 */	
			if(RTU_FLAG==1)//�յ�һ֡��������
			{
				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
				{
					if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
					{
						switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
						memset(RS485_RX_BUF, 0, 64);//������ջ�����������
						RS485_RX_CNT=0;//�������¼���
						Timer=0;//��ʱ����������

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
						{
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer = 0;//��ʱ����������
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
	
    	//���Ҫ���͵ı���
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

    USART_Send_Buffer(buffer, 13);//��������

    osDelay(1);//��ʱ�ȴ��������
	
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG==1)//�յ�һ֡��������
			{
				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
				{
					if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
					{
						switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
						memset(RS485_RX_BUF, 0, 64);//������ջ�����������
						RS485_RX_CNT=0;//�������¼���
						Timer=0;//��ʱ����������

						return error;
					}
					else
					{
						if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
						{
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer = 0;//��ʱ����������
								break;
						}
					}
					
				}
				memset(RS485_RX_BUF, 0, 64);//������ջ�����������
				RS485_RX_CNT=0;//�������¼���
				RTU_FLAG=0;//���������ɱ�־λ
			}
    }
		
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}

	//���Ҫ���͵ı���
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
    USART_Send_Buffer(buffer, 11);//��������
    osDelay(1);//��ʱ�ȴ��������
		Timer = 0;//��ʱ����������
    while(Timer < 2000)//2s��ʱ
    {
			if(RTU_FLAG == 1)//�յ�һ֡��������
			{
					if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
									switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									Timer=0;//��ʱ����������
									return error;//���ع�����
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
									{
											memset(RS485_RX_BUF, 0, 64);//������ջ�����������
											RS485_RX_CNT=0;//�������¼���
											Timer = 0;//��ʱ����������
											break;
									}
							}
						}
					memset(RS485_RX_BUF, 0, 64);//������ջ�����������
					RS485_RX_CNT=0;//�������¼���
					RTU_FLAG=0;//���������ɱ�־λ
				}
    }
		memset(RS485_RX_BUF, 0, 64);//������ջ�����������
		if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
		{
			Timer=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			Timer = 0;//��ʱ����������
			return RETURN_SUCCESS;//���سɹ�
		}
 
}

/********************************************************
  * @brief      : NiM_goHome
  * @details    : ԭ��ع�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 1~127
	                nType��		 ԭ��ع鷽ʽ      ȡֵ��ο���һ�廯�������CANͨѶ�û��ֲ�(�ջ�).pdf����9�¶���Ŀ¼����
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_goHome(uint32_t nAddr, uint32_t nType)
{
		uint32_t error=0;//����״̬����ֵ
    uint16_t crc16 = 0;//16λCRCУ����
		 if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}		
		error=NiM_writeParam(nAddr, 0x6B, 0x02, nType);
	  if(error != 0)
		{
				return error;
		}
		else
		{
					//���Ҫ���͵ı���
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

				USART_Send_Buffer(buffer, 11);//��������

				osDelay(1);//��ʱ�ȴ��������
			
				Timer = 0;//��ʱ����������
				while(Timer < 2000)//2s��ʱ
				{
					/* BEGIN:   PN:3 */	
					if(RTU_FLAG==1)//�յ�һ֡��������
					{
						if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
						{
							if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
							{
								switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								Timer=0;//��ʱ����������

								return error;
							}
							else
							{
								if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
								{
										memset(RS485_RX_BUF, 0, 64);//������ջ�����������
										RS485_RX_CNT=0;//�������¼���
										Timer = 0;//��ʱ����������
										break;
								}
							}
							
						}
						memset(RS485_RX_BUF, 0, 64);//������ջ�����������
						RS485_RX_CNT=0;//�������¼���
						RTU_FLAG=0;//���������ɱ�־λ
					}
				}
				
				if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
				{
					Timer=0;//��ʱ����������
					return TIMEOUT_ERROR;//���س�ʱ
				}
				
						//���Ҫ���͵ı���
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
					USART_Send_Buffer(buffer, 11);//��������
					osDelay(1);//��ʱ�ȴ��������
					Timer = 0;//��ʱ����������
					while(Timer < 2000)//2s��ʱ
					{
						if(RTU_FLAG == 1)//�յ�һ֡��������
						{
								if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
									{
										if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
										{
												switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
												memset(RS485_RX_BUF, 0, 64);//������ջ�����������
												RS485_RX_CNT=0;//�������¼���
												Timer=0;//��ʱ����������
												return error;//���ع�����
											/* END:   PN:3 */	
										}
										else
										{

												if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[3] == buffer[3]) && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
												{
														memset(RS485_RX_BUF, 0, 64);//������ջ�����������
														RS485_RX_CNT=0;//�������¼���
														Timer = 0;//��ʱ����������
														break;
												}
										}
									}
								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
								RS485_RX_CNT=0;//�������¼���
								RTU_FLAG=0;//���������ɱ�־λ
							}
					}
					memset(RS485_RX_BUF, 0, 64);//������ջ�����������
					if(Timer >=2000)//�ж��Ƿ���ճ�ʱ
					{
						Timer=0;//��ʱ����������
						return TIMEOUT_ERROR;//���س�ʱ
					}
					else 
					{
						Timer = 0;//��ʱ����������
						return RETURN_SUCCESS;//���سɹ�
					}
		}
	
}

/********************************************************
  * @brief      : NiM_powerOn
  * @details    : �����������·�ϵ磨������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~127

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_powerOn(uint32_t nAddr)
{
		if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}	
		return NiM_writeParam(nAddr,0x0051,2,0x0007);
}

/********************************************************
  * @brief      : NiM_powerOff
  * @details    : �����������·�ϵ磨�ѻ���
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~127

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_powerOff(uint32_t nAddr)
{
		 if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}		
		return NiM_writeParam(nAddr,0x0051,2,0x0006);
}

/********************************************************
  * @brief      : NiM_stop
  * @details    : ֹͣ��ǰ����
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~127

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_stop(uint32_t nAddr)
{
		 if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}		
	  return NiM_writeParam(nAddr,0X0051,2,0X007);
}

/********************************************************
  * @brief      : NiM_fastStop
  * @details    : ��ͣ
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 1~127

  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/6/17
  *	@author     : Nimotion

  *
  ********************************************************/
uint32_t NiM_fastStop(uint32_t nAddr)
{
		if(ActionFlag!=0)//�ж��˶�ָ��ʹ�ܱ�־λ�Ƿ���Ч
		{
			return ACKNOWLEDGE;//���ؾ���λ������δ����
		}	
		return NiM_writeParam(nAddr,0X0051,2,0X002);;
}


///**** Copyright (C)2018 Nimotion All Rights Reserved **** END OF FILE ****/

