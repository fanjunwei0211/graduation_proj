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
// Modbus ��ȡ���ݻظ�
volatile int sdo_recv_cnt = 0;
extern volatile uint16_t RTU_FLAG;
//���ջ����� 	
extern uint8_t RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
extern uint8_t RS485_RX_CNT; 
extern void delay_ms(u16 nms);

uint8_t buffer[20] = {0};
volatile int timer6_cnt;
extern void delay_ms(u16 nms);


/********************************************************
  * @brief      : NiM_isMotorOnline
  * @details    : �жϵ���Ƿ�����
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
	                
  * @param [out]: *pOnline  ��������     ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @retval     :0 �ɹ���������ʾ������
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
  *
  ********************************************************/
uint8_t NiM_isMotorOnline(uint8_t nAddr, uint16_t *pOnline)
{
    if(NiM_readParam(nAddr, 0x06,(uint16_t *) pOnline,2) == 0)//�ж��ܷ��ȡ�������
    {

        return 0;
    }
    else
    {
        *pOnline = 0;//���������
        return 1;
    }
}

/********************************************************
  * @brief      : NiM_getMotorInfo
  * @details    : ��ȡ���������Ϣ
  * @param [in] : nAddr�� �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: pInfo�� �����Ϣ
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ��ȡ���������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: pErrorCode�� ���ش����� 	ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ��ȡ���������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: pVelocity �����ٶ� 	ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ����������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
 
  * History
  * @date       : 2022/9/28
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
  *
  ********************************************************/
uint8_t NiM_clearErrorState(uint8_t nAddr)
{
	uint16_t crc16 = 0;//16λCRCУ����
	uint8_t buffer[8];
	//���Ҫ���͵ı���
	buffer[0] = nAddr;
	buffer[1] = 0x06;
	//�жϼĴ����ĵ�ַ

	buffer[2] = 0x00;
	buffer[3] = 0x51;

	buffer[4] = 0x00;	
	buffer[5] = 0x80;
	crc16 = usMBCRC16(buffer, 6);
	buffer[6] = (uint8_t)(crc16 & 0xff);
	buffer[7] = (uint8_t)((crc16 >> 8) & 0xff);

	USART3_Send_Buffer(buffer, 8);//��������
	return 0;
}





/********************************************************
  * @brief      : NiM_readParam
  * @details    : ��ȡ�������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ��0~247   
									index������         ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲᡷ��10�¶����ֵ�    
									subindex:������     ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲᡷ��10�¶����ֵ�
									len�����󳤶�  1��ʾ1���ֽڣ�2-2���ֽڣ�4-4���ֽ� ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @param [out]: *date     ��������      ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @retval     :0 �ɹ���������ʾ������

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
  *
  ********************************************************/
uint16_t fvalv=0;
//nAddr,0x1018,0x03,(uint8_t *)&pInfo->szSoftVersion,4
uint8_t NiM_readParam(uint8_t nAddr, uint16_t index, uint16_t *data, uint8_t len)
{
	uint32_t i=0;
	uint8_t addi=0;
	uint32_t error=0;//����״̬����ֵ
  uint16_t crc16 = 0;//16λCRCУ����

	//���Ҫ���͵ı���
   buffer[0] = nAddr;
	 buffer[1] = 0x04;
	 //�жϼĴ����ĵ�ַ

		buffer[2] = (index>>8)&0xff;
		buffer[3] = index&0xff;
		buffer[4] = 0x00;
    buffer[5] = len / 2;
		crc16 = usMBCRC16(buffer, 6);
    buffer[6] = (uint8_t)(crc16 & 0xff);
    buffer[7] = (uint8_t)((crc16 >> 8) & 0xff);

    USART3_Send_Buffer(buffer, 8);//��������
    delay_ms(1);//��ʱ�ȴ��������
	
		timer6_cnt = 0;//��ʱ����������
    while(timer6_cnt < 500)//2s��ʱ
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
									timer6_cnt=0;//��ʱ����������
									return error;//���ع�����
								/* END:   PN:3 */	
							}
							else
							{

									if((RS485_RX_BUF[1] == buffer[1]) && (RS485_RX_BUF[2] == len))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
								{
									//�洢�����Ϣ����
									for(i=0;i<(len/2);i++)
									{
										//*data = (uint32_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
										*data = (uint32_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
										 data++;
										// fvalv= (uint16_t)((RS485_RX_BUF[3+i*2] << 8) + (RS485_RX_BUF[4+i*2] << 0));
									}
									
									memset(RS485_RX_BUF, 0, 64);//������ջ�����������
									RS485_RX_CNT=0;//�������¼���
									timer6_cnt = 0;//��ʱ����������
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
		if(timer6_cnt >=2000)//�ж��Ƿ���ճ�ʱ
		{
			timer6_cnt=0;//��ʱ����������
			return TIMEOUT_ERROR;//���س�ʱ
		}
		else 
		{
			timer6_cnt = 0;//��ʱ����������
			return RETURN_SUCCESS;//���سɹ�
		}	

}

/********************************************************
  * @brief      : NiM_writeParam
  * @details    : ���õ������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									index������         ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
									subindex:������     ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
									len�����󳤶�  1��ʾ1���ֽڣ�2-2���ֽڣ�4-4���ֽ� ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
									date������ֵ 
  * @param [out]: ��         
  * @retval     :0 �ɹ���������ʾ������

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
  *
  ********************************************************/
 /********************************************************/


//uint8_t NiM_writeParam(uint8_t nAddr,uint16_t index,uint8_t *data, uint8_t len)
//{
//	uint32_t i=0;
//	uint32_t error=0;//����״̬����ֵ
//  uint16_t crc16 = 0;//16λCRCУ����

//	//���Ҫ���͵ı���
//	buffer[0] = nAddr;

//	//�ж�дһ���Ĵ�������д����Ĵ���
//	buffer[1] = 0x10;
//	//�жϼĴ����ĵ�ַ

//	buffer[2] = (index>>8)&0xff;
//	buffer[3] = index&0xff;

//	//00 01 02    00 02 04
//	//�жϼĴ�������
//	buffer[4] = 0x00;	
//	if(len>2)
//	{
//		buffer[5] = 0x02;
//	}
//	else
//	{
//		buffer[5] = 0x01;
//	}	
//	//1���ֽ�λ
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
//		//������Ϣ
//		
//		buffer[7] = data[3];
//		buffer[8] = data[2];
//		buffer[9] = data[1];
//		buffer[10]= data[0];
//		crc16 = usMBCRC16(buffer, 11);
//		buffer[11] = (uint8_t)(crc16 & 0xff);
//		buffer[12] = (uint8_t)(crc16 >> 8);
//		USART3_Send_Buffer(buffer, 13);//��������
//		delay_ms(1);//��ʱ�ȴ��������
//	}
//	else
//	{
//			//������Ϣ
//		buffer[7] = data[1];
//		buffer[8]= data[0];
//		crc16 = usMBCRC16(buffer, 9);
//		buffer[9] = (uint8_t)(crc16 & 0xff);
//		buffer[10] = (uint8_t)(crc16 >> 8);
//		USART3_Send_Buffer(buffer, 11);//��������
//		delay_ms(1);//��ʱ�ȴ��������
//	}

////	/////////////////////////////////////////////
//	timer6_cnt = 0;//��ʱ����������
//	RTU_FLAG=0;//���������ɱ�־λ
//	HAL_TIM_Base_Start_IT(&htim7);
//	while(timer6_cnt < 500)//2s��ʱ
//	{
//		/* BEGIN:   PN:3 */	
//		if(RTU_FLAG == 1)//�յ�һ֡��������
//		{ 
//				if(RS485_RX_BUF[0] == nAddr)//�жϽ��յ��������ǲ�����ȷ�ĵ��������
//				{
//						if(RS485_RX_BUF[1] > 0x80)//�жϵ���Ƿ����
//						{
//								switch(RS485_RX_BUF[2])//���������쳣��������Ӧ���쳣״̬
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
//								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
//								RS485_RX_CNT=0;//�������¼���
//								timer6_cnt=0;//��ʱ����������
//								return error;//���ع�����
//							/* BEGIN:   PN:3 */	
//						}
//						else
//						{
//							
//							if((RS485_RX_BUF[1] == buffer[1])&& (RS485_RX_BUF[3] == buffer[3])  && (RS485_RX_BUF[5] == buffer[5]))//�ڵ���ǹ���״̬�£��жϽ��յ��������Ƿ�������
//							{
//								
//								memset(RS485_RX_BUF, 0, 64);//������ջ�����������
//								RS485_RX_CNT=0;//�������¼���
//								timer6_cnt = 0;//��ʱ����������
//								break;
//							}


//						}
//				}
//			memset(RS485_RX_BUF, 0, 64);//������ջ�����������
//			RS485_RX_CNT=0;//�������¼���
//			RTU_FLAG=0;//���������ɱ�־λ
//		}
//	}
//	memset(RS485_RX_BUF, 0, 64);//������ջ�����������
//	if(timer6_cnt >=2000)//�ж��Ƿ���ճ�ʱ
//	{
//		timer6_cnt=0;//��ʱ����������
//		return TIMEOUT_ERROR;//���س�ʱ
//	}
//	else 
//	{
//		timer6_cnt = 0;//��ʱ����������
//		return RETURN_SUCCESS;//���سɹ�
//	}
//}

uint8_t NiM_writeParam(uint8_t nAddr,uint16_t index,uint8_t *data, uint8_t len)
{
	uint32_t i=0;
	uint32_t error=0;//����״̬����ֵ
  uint16_t crc16 = 0;//16λCRCУ����

	//���Ҫ���͵ı���
	buffer[0] = nAddr;

	//�ж�дһ���Ĵ�������д����Ĵ���
	buffer[1] = 0x10;
	//�жϼĴ����ĵ�ַ

	buffer[2] = (index>>8)&0xff;
	buffer[3] = index&0xff;

	//00 01 02    00 02 04
	//�жϼĴ�������
	buffer[4] = 0x00;	
	if(len>2)
	{
		buffer[5] = 0x02;
	}
	else
	{
		buffer[5] = 0x01;
	}	
	//1���ֽ�λ
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
		//������Ϣ
		
		buffer[7] = data[3];
		buffer[8] = data[2];
		buffer[9] = data[1];
		buffer[10]= data[0];
		crc16 = usMBCRC16(buffer, 11);
		buffer[11] = (uint8_t)(crc16 & 0xff);
		buffer[12] = (uint8_t)(crc16 >> 8);
		USART3_Send_Buffer(buffer, 13);//��������
//		delay_ms(1);//��ʱ�ȴ��������
	}
	else
	{
			//������Ϣ
		buffer[7] = data[1];
		buffer[8]= data[0];
		crc16 = usMBCRC16(buffer, 9);
		buffer[9] = (uint8_t)(crc16 & 0xff);
		buffer[10] = (uint8_t)(crc16 >> 8);
		USART3_Send_Buffer(buffer, 11);//��������
//		delay_ms(1);//��ʱ�ȴ��������
	}

//	/////////////////////////////////////////////
	RTU_FLAG=0;//���������ɱ�־λ
	HAL_TIM_Base_Start_IT(&htim7);
	/* BEGIN:   PN:3 */	
	while(!RTU_FLAG);
	return RETURN_SUCCESS;//���سɹ�
}

/********************************************************
  * @brief      : NiM_isTargetReached
  * @details    : �ж�λ�õ���
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: tr    0��δ����    1������
  * @retval     :0 �ɹ���������ʾ������
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ����������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : �ָ������������
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : �޸ĵ����ַ
  * @param [in] : nCurAddr��   �����ǰ��ַNode-ID   ȡֵ��Χ�� 0~247   
									nNewAddr��   ����µ�ַNode-ID     ȡֵ��Χ�� 0~247   
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : �޸ĵ������ģʽ
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									nMode��   �������ģʽ      ȡֵ��Χ��	0~6  ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ��ȡ���״̬��
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
	                             
  * @param [out]: pStatusWord��  ����״̬��		ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @retval     :0 �ɹ���������ʾ������
 
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ��ȡ�����ǰλ��
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
	                             
  * @param [out]: pPosition��  ���ص�ǰλ��		ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ����λ���˶�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 0~247   
									nType��     ����ָ��ʱ��0--�������£�1--���������� ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
	                nPosition��Ŀ��λ��          ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : �������ͣ�Modbus
								����ִ��֮ǰ��ȷ�����㣺
											                 1���������λ���˶�ģʽ��
											                 2�����ʹ��
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
  * @details    : ���λ���˶�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 0~247   
									nType��     ����ָ��ʱ��0--�������£�1--���������� ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
	                nDistance��Ŀ��λ��          ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : �������ͣ�Modbus
								����ִ��֮ǰ��ȷ�����㣺
											                 1���������λ���˶�ģʽ��
											                 2�����ʹ��
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
			//Ԥ���Ŀ��λ�ã��û���λ��
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
  * @details    : �ٶ�ģʽ�˶�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 0~247               
                  nVelocity��Ŀ���ٶ�          ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : �������ͣ�Modbus
								����ִ��֮ǰ��ȷ�����㣺
											                 1����������ٶ��˶�ģʽ��
											                 2�����ʹ��
  *
  ********************************************************/
//error = NiM_writeParam(nAddr,0x2017,0x01,(uint8_t *)&temp2,2);//�������ʹ��
uint8_t NiM_moveVelocity(uint8_t nAddr, int32_t nVelocity)
{
	uint8_t error;
	static char isPower=0;
	uint16_t temp=0;
	int32_t tVelocity = nVelocity;
//	if(isPower==0){
//		isPower=1;
//		temp = 0x06;
//		//601 2B 40 60 00 06 00 00 00  ʹ���׼��
//		NiM_writeParam(nAddr,0x51,(uint8_t *)&temp,2);
//		temp = 0x07;
//		//601 2B 40 60 00 07 00 00 00 ʹ���ʧ��
//		NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//		temp = 0x0F;
//		//601 2B 40 60 00 0F 00 00 00 ʹ���ʹ��
//		NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//	}
		
	if(nVelocity>0)
	{
		temp=0x01;//1->������
		error = NiM_writeParam(nAddr,0x52,(uint8_t *)(&temp),2);
		if(error ==0)
		{
			//�ٶ�����
			error = NiM_writeParam(nAddr,0x55,(uint8_t *)(&nVelocity),4);
		}
	}
	else if(nVelocity<0)
	{
		temp=0x00;//0->������
		error = NiM_writeParam(nAddr,0x52,(uint8_t *)(&temp),2);
		if (error ==0)
		{
			//�ٶ�����
			tVelocity=-nVelocity;
			error = NiM_writeParam(nAddr,0x55,(uint8_t *)(&tVelocity),4);
		}
	}
	return 0;
}

/********************************************************
  * @brief      : NiM_moveTorque
  * @details    : ת��ģʽ�˶�
  * @param [in] : nAddr��  	 �����ַNode-ID   ȡֵ��Χ�� 0~247               
                  nTorque��  Ŀ��ת��          ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf����10�¶����ֵ� 
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : �������ͣ�Modbus
								����ִ��֮ǰ��ȷ�����㣺
											                 1���������ת���˶�ģʽ��
											                 2�����ʹ��
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
  * @details    : ԭ��ع�ģʽ�˶�
  * @param [in] : nAddr��  	 �����ַNode-ID       ȡֵ��Χ�� 0~247               
                  nType��    ԭ��ع鷽ʽ          ȡֵ������ο���PMM60L��STM-M��BLM ϵ��һ�廯���Modbus ͨ�ţ��ջ����û��ֲ�.pdf�� ��5.5.7�¹�������
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������
  
  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note     : �������ͣ�Modbus
								����ִ��֮ǰ��ȷ�����㣺
											                 1���������ԭ��ع��˶�ģʽ��
											                 2�����ʹ��
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
  * @details    : ���ʹ��
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
  *
  ********************************************************/
//uint8_t NiM_powerOn(uint8_t nAddr)
//{
//	uint16_t temp=0;
//	uint8_t error;
//	temp = 0x06;
//	//601 2B 40 60 00 06 00 00 00  ʹ���׼��
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)&temp,2);
//	if (error !=0)
//		return error;
//	temp = 0x07;
////	delay_ms(2);
//	//601 2B 40 60 00 07 00 00 00 ʹ���ʧ��
//	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
//	if (error !=0)
//		return error;
////	delay_ms(2);
//	temp = 0x0F;
//	//601 2B 40 60 00 0F 00 00 00 ʹ���ʹ��
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
	//601 2B 40 60 00 06 00 00 00  ʹ���׼��
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)&temp,2);
	if (error !=0)
		return error;
	temp = 0x07;
	//601 2B 40 60 00 07 00 00 00 ʹ���ʧ��
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	temp = 0x0F;
	//601 2B 40 60 00 0F 00 00 00 ʹ���ʹ��
	error = NiM_writeParam(nAddr,0x51,(uint8_t *)(&temp),2);
	if (error !=0)
		return error;
	return 0;
}

/********************************************************
  * @brief      : NiM_powerOff
  * @details    : ����ѻ�
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ֹͣ��ǰ�˶�
  * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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
  * @details    : ��ͣ
   * @param [in] : nAddr��   �����ַNode-ID   ȡֵ��Χ�� 0~247   
									
  * @param [out]: ��
  * @retval     :0 �ɹ���������ʾ������

  * History
  * @date       : 2020/12/1
  *	@author     : Nimotion
  * @note       : �������ͣ�Modbus
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



