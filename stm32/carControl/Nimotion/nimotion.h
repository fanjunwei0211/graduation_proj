#ifndef __NIMOTION_H
#define __NIMOTION_H	 
#include "main.h"

#define SUCCESS         0       // ��ȡ����д����ȷ
#define WRONG_LENGTH   1       // ������������ݳ��ȴ���
#define WRONG_RESPONSE   2     //���յ�����Ļ�Ӧ ���������� CRC�����
#define EXCEPTION_RESPONSE   3 // ����������ָ����� 
#define TIME_OUT             4 // ����һ��ʱ��û���յ��ظ����ظ�������ȷ�Ļظ��ʹ���ظ�
#define WRONG_SEQUENCE       5 //��������Ŀ���ʱ�����


#define RETURN_SUCCESS 0              //���ճɹ�
#define TIMEOUT_ERROR 7               //��ʱ����
#define ILLEGAL_FUNCTION 1            //�Ƿ�������
#define ILLEGAL_DATA_ADDRESS 2        //�Ƿ����ݵ�ַ
#define ILLEGAL_DATA_VALUE  3         //�Ƿ�����ֵ
#define SLAVE_DEVICE_FAILURE 4        //�ӻ��豸����
#define ACKNOWLEDGE 5                 //�ӻ���Ӧʱ���������Ҫ��������ѯ��
#define SLAVE_BUSY 6                  //���豸æµ�������ڴ��豸��״̬ʱ�ٷ���
#define MOTOR_ALARM 8                 //����ڲ��б���

#define NMT	   0x0
#define SYNC       0x1
#define TIME_STAMP 0x2
#define PDO1tx     0x3
#define PDO1rx     0x4
#define PDO2tx     0x5
#define PDO2rx     0x6
#define PDO3tx     0x7
#define PDO3rx     0x8
#define PDO4tx     0x9
#define PDO4rx     0xA
#define SDOtx      0xB
#define SDOrx      0xC
#define NODE_GUARD 0xE
#define LSS 	   0xF

extern int tr ;
extern int a ;
extern volatile int timer6_cnt;
//�˴�Ϊ�ṹ�嶨��
typedef struct _MOTOR_INFO
{
    uint8_t nAddr;  //�����ַ
    uint32_t szSerialNumber;    //������к�
    uint32_t szSoftVersion;     //����汾��
} MOTOR_INFO,*P_MOTOR_INFO;



typedef enum _WORK_MODE
{
    POSITION_MODE = 1,
    VELOCITY_MODE = 2,
		TORQUE_MODE = 4,
    GOHOME_MODE = 6
} WORK_MODE;




//�˴�Ϊ��������

/**
 * @brief �жϵ���Ƿ�����
 * @param nAddr �����ַ
 * @param pOnline ָ�룬��������״̬
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_isMotorOnline(uint8_t nAddr,uint16_t *pOnline);
/**
 * @brief �жϵ���Ƿ񵽴�Ŀ��λ��
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_isTargetReached(uint8_t nAddr);
/**
 * @brief ��ȡ���������Ϣ
 * @param nAddr �����ַ
 * @param pInfo �����Ϣ�ṹ��ָ��
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_getMotorInfo(uint8_t nAddr, MOTOR_INFO* pInfo);

/**
 * @brief ��ȡ���������
 * @param nAddr �����ַ
 * @param pErrorCode ָ�룬���ش���Ĵ���
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_getErrorCode(uint8_t nAddr, uint16_t *pErrorCode);

/**
 * @brief ����������
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_clearErrorState(uint8_t nAddr);

/**
 * @brief ��ȡ�������ֵ
 * @param nAddr �����ַ
 * @param index ��������
	 @param len ���󳤶�
 * @param data ָ�룬���ز���ֵ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_readParam(uint8_t nAddr, uint16_t index, uint16_t *data, uint8_t len);

/**
 * @brief ���õ������ֵ
 * @param nAddr �����ַ
 * @param index ��������
	 @param len ���󳤶�
 * @param data ָ�룬д�����ֵ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_writeParam(uint8_t nAddr,uint16_t index, uint8_t *data, uint8_t len);

/**
 * @brief ����������
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_saveParams(uint8_t nAddr);

/**
 * @brief �ָ������������
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_restoreFactorySettings(uint8_t nAddr);

/**
 * @brief �޸ĵ����ַ
 * @param nCurAddr �����ǰ��ַ
 * @param nNewAddr �޸ĺ���µ�ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_changeAddr(uint8_t nCurAddr, uint8_t nNewAddr);

/**
 * @brief �޸ĵ������ģʽ
 * @param nAddr �����ַ
 * @param nMode ����ģʽ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_changeWorkMode(uint8_t nAddr, WORK_MODE nMode);

/**
 * @brief ��ȡ���״̬��
 * @param nAddr �����ַ
 * @param pStatusWord ָ�룬����״̬��
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_getCurrentStatus(uint8_t nAddr, uint16_t *pStatusWord);

/**
 * @brief ��ȡ�����ǰλ��
 * @param nAddr �����ַ
 * @param pPosition ָ�룬���ص�ǰλ��
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_getCurrentPosition(uint8_t nAddr, int32_t *pPosition);

/**
 * @brief ����λ���˶�
 * @param nAddr �����ַ
 * @param nType 0-��������/1-����������
 * @param nPosition Ŀ��λ��
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_moveAbsolute(uint8_t nAddr, uint8_t nType,int32_t nPosition);

/**
 * @brief ���λ���˶�
 * @param nAddr �����ַ
 * @param nType ����ָ��ʱ�� 
 * @param nDistance �˶�����
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_moveRelative(uint8_t nAddr, uint8_t nType,int32_t nDistance);

/**
 * @brief �ٶ�ģʽ�˶�
 * @param nAddr �����ַ
 * @param nVelocity Ŀ���ٶ�
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_moveVelocity(uint8_t nAddr, int32_t nVelocity);

/**
 * @brief ת��ģʽ�˶�
 * @param nAddr �����ַ
 * @param nTorque Ŀ��ת��
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_moveTorque(uint8_t nAddr, int16_t nTorque);

/**
 * @brief ԭ��ع�
 * @param nAddr �����ַ
 * @param nType ԭ��ع鷽ʽ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_goHome(uint8_t nAddr, int8_t nType);

/**
 * @brief ���ʹ��
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_powerOn(uint8_t nAddr);

/**
 * @brief ����ѻ�
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_powerOff(uint8_t nAddr);

/**
 * @brief ֹͣ��ǰ�˶�
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_stop(uint8_t nAddr);

/**
 * @brief ��ͣ
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */
uint8_t NiM_fastStop(uint8_t nAddr);


 /**
 * @brief ʵ���ٶȻ�ȡ
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */

uint8_t NiM_Velocity(uint8_t nAddr, uint32_t *pVelocity);
 /**
 * @brief ʵ���ٶȻ�ȡ
 * @param nAddr �����ַ
 * @return 0 �ɹ���������ʾ������
 */

//***************************************************************************************************************************************





















#endif

