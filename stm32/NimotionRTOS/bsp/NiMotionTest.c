/******************************************************************************
  *
  *   Copyright (C), 2010-2018, Beijing NiMotion Control Technology Co., Ltd.
  *
  *****************************************************************************
  *@file       : NiMotionTest.c
  *@version    : Initial Draft
  *@author     : Nimotion
  *@date       : 2019/7/12
  *@brief      :
  *****************************************************************************
  **function   :
								������Ϊ��������ʤ���Ƽ������޹�˾����MODBUSͨ�ŵ��ʹ�ð���


								�������ȨΪ��������ʤ���Ƽ������޹�˾����
								δ������˾��������ת��������ҵ��;
								����Ƿ�ת�����һ�������¹ʣ����뱾��˾���κ����ι�ϵ
  *History     :
  *@date       : 2019/7/12
  *@author     : Nimotion
  *@note       : Created file

  ****************************************************************************/
#include "NiMotionTest.h"
#include "NiMotionMotorSDK.h"
#include "string.h"
#include "cmsis_os.h"

MOTOR_INFO Information = {0}; //�����洢�����Ϣ
uint32_t buffer1[10];//�����洢���ϲ���
uint32_t buffer2[10];//�����洢���λ��
uint32_t AppFlag;
/********************************************************
  * @brief      : NiM_Test_Position_Mode
  * @details    : ���λ��ģʽ����
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test_Position_Mode(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;
    /*******************���λ��ģʽ���а���***************/
		nim_return = NiM_changeWorkMode(Motor, POSITION_MODE); //�л���λ��ģʽ
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 1;//��һ������
			return error_step;
		}
		osDelay(100);//�ȴ�1s
		nim_return = NiM_saveAsHome(Motor); //����ԭ��
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 2;//�ڶ�������
			return error_step;
		}
		osDelay(100);//�ȴ�1s
		nim_return = NiM_powerOff(Motor); //�ѻ�
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 3;//����������
			return error_step;
		}
		osDelay(100);//�ȴ�1s
		nim_return = NiM_powerOn(Motor); //����
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 4;//���Ĳ�����
			return error_step;
		}
		osDelay(100);//�ȴ�1s
		*TAGPostion=5000000;
		nim_return = NiM_moveAbsolute(Motor, 5000000); //����λ�����е�3200
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 5;//���岽����
			return error_step;
		}
		NiM_clearAlarms(Motor1);//���1�ŵ������
		NiM_clearErrorState(Motor1);//���1�ŵ������״̬

		nim_return = NiM_moveRelative(Motor, 1000000); //���λ����������1600
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 6;//����������
			return error_step;
		}
		osDelay(1000);//�ȴ�10s
		nim_return = NiM_moveRelative(Motor, -1000000); //���λ�÷�������1600
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 7;//���߲�����
			return error_step;
		}
		osDelay(1000);//�ȴ�10s
		
		return 0;		
}

/********************************************************
  * @brief      : NiM_Test_Velocity_Mode
  * @details    : ����ٶ�ģʽ����
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test_Velocity_Mode(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;

    /*******************����ٶ�ģʽ���а���***************/
   
		nim_return = NiM_changeWorkMode(Motor, VELOCITY_MODE); //�л����ٶ�ģʽ
 		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			
			error_step = 1;//��һ������
			return error_step;
		}
		NiM_AppLocation(Motor);//Ӧ���Թ��ϴ���
		osDelay(100);//�ȴ�1s
    nim_return = NiM_powerOff(Motor);//�ѻ�
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			
			error_step = 2;//�ڶ�������
			return error_step;
		}
		NiM_AppLocation(Motor);//Ӧ���Թ��ϴ���
		osDelay(100);//�ȴ�1s
    nim_return = NiM_powerOn(Motor);//����
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			
			error_step = 3;//����������
			return error_step;
		}
		NiM_AppLocation(Motor);//Ӧ���Թ��ϴ���
		osDelay(100);//�ȴ�1s
    nim_return = NiM_moveVelocity(Motor, 100); //�ٶ�100��ת
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			
			error_step = 4;//���Ĳ�����
			return error_step;
		} 
		NiM_AppLocation(Motor);//Ӧ���Թ��ϴ���
		osDelay(1000);//�ȴ�10s
    nim_return = NiM_moveVelocity(Motor, -200); //�ٶ�200��ת
    if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 5;//���岽����
			return error_step;
		}
		NiM_AppLocation(Motor);//Ӧ���Թ��ϴ���
		osDelay(1000);//�ȴ�10s
    nim_return = NiM_powerOff(Motor);//�ѻ�
    if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 6;//����������
			return error_step;
		}
		NiM_AppLocation(Motor);//Ӧ���Թ��ϴ���
		osDelay(100);//�ȴ�1s
		return 0;
}
/********************************************************
  * @brief      : NiM_Test_Alarm_Mode
  * @details    : �����������
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test_Alarm_Mode(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;
    /*******************������ϱ������������***************/
    nim_return = NiM_writeParam(Motor, 0x1b, 0x02, 20);//��ѹ����Ԥ��ֵ��Ϊ20
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 1;//��һ������
			return error_step;
		}
    osDelay(1000);//�ȴ�10s
    nim_return = NiM_writeParam(Motor, 0x1b, 0x02, 24);//��ѹ����Ԥ��ֵ��Ϊ24
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 2;//�ڶ�������
			return error_step;
		}
    osDelay(1000);//�ȴ�10s
    nim_return = NiM_clearAlarms(Motor);//�������
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 3;//����������
			return error_step;
		}
    osDelay(1000);//�ȴ�10s
    nim_return = NiM_clearErrorState(Motor);//�������״̬
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 4;//���Ĳ�����
			return error_step;
		}
    osDelay(1000);//�ȴ�10s
		return 0;
}

/********************************************************
  * @brief      : NiM_Test
  * @details    : �����Ϣ��ȡ����
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/

uint32_t NiM_Test(uint32_t Motor)
{
		uint32_t error_step = 0;
		uint32_t nim_return = 0;	
    /*******************����汾��Ϣ***************/
    nim_return = NiM_getMotorInfo(Motor, &Information);//��ȡ����汾��Ϣ
		if(nim_return != 0)//�������ִ�д����˳��˺��������ص�ǰ����
		{
			error_step = 1;//��һ������
			return error_step;
		}	
    osDelay(1000);//�ȴ�10s
		return 0;
}
/* BEGIN:   PN:4 */	
/********************************************************
  * @brief      :	NiM_AppLocation
  * @details    : Ӧ���Թ��ϴ���
  * @param [in] :
  * @param [out]: None
  * @retval     :
  * Calls       :
  * Called By   :

  * History
  * @date       : 2019/7/8
  * @author     : Nimotion
  *   @note     : Created function
  *
  ********************************************************/
void NiM_AppLocation(uint32_t Motor)
{
	uint32_t i;
	NiM_getHistoryAlarms(Motor,buffer1,&buffer2[9]);//��ȡ��ʷ���ϱ���
	if(buffer2[9]!=0)//������ڱ���
	{
		for(i=0;i<buffer2[9];i++)
		{
			switch (buffer1[i])
			{
				case OVER_CURRENT://����
					;
				case OVER_VOLTAGE://��ѹ
					;
				case UNDER_VOLTAGE://Ƿѹ
					;
				case TEMP_OVER://���ȹػ�
					NiM_clearAlarms(Motor);//�������
					NiM_clearErrorState(Motor);//�������״̬
				break;
				default:
					break;
			}
		}
		memset(buffer1,0,10);//��ձ���ֵ�洢����
		AppFlag=1;
	}
}
/* END:   PN:4 */	


/**** Copyright (C)2018 Nimotion All Rights Reserved **** END OF FILE ****/

