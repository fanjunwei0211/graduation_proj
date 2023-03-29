#include "laser.h"
static unsigned char TxBuffer[256];
static unsigned char count=0; 
int laserDis[3]={0,0,0};

void UART1_PutChar(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
  while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
}

//040300030001745F   4����������
//050300030001758E   5����������
//06030003000175BD	 6���������
//060300020001247D   �򿪼���
//060300020000E5BD   �رռ���
//LaseeSerialDataΪ�����жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void LaserSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;	//���յ������ݴ��뻺������
	if ((ucRxBuffer[0]!=0x04) && (ucRxBuffer[0]!=0x05) && (ucRxBuffer[0]!=0x06)) //����ͷ���ԣ������¿�ʼѰ������ͷ
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<8) {return;}//���ݲ���11�����򷵻�
	else
	{
		switch(ucRxBuffer[0])//�ж��������ĸ��������Ƿ��ص�
		{
			case 0x04:	memcpy(&laserDis[0],&ucRxBuffer[4],2);break;//memcpyΪ�������Դ����ڴ濽��������������"string.h"�������ջ��������ַ����������ݽṹ�����棬�Ӷ�ʵ�����ݵĽ�����
			case 0x05:	memcpy(&laserDis[1],&ucRxBuffer[4],2);break;
			case 0x06:	memcpy(&laserDis[2],&ucRxBuffer[4],2);break;
		}
		ucRxCnt=0;//��ջ�����
	}
}