#include "laser.h"
static unsigned char TxBuffer[256];
static unsigned char count=0; 
int laserDis[3]={0,0,0};

void UART1_PutChar(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
  while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC)!=SET);		//等待发送结束
}

//040300030001745F   4号连续发送
//050300030001758E   5号连续发送
//06030003000175BD	 6号连续测距
//060300020001247D   打开激光
//060300020000E5BD   关闭激光
//LaseeSerialData为串口中断调用函数，串口每收到一个数据，调用一次这个函数。
void LaserSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
	if ((ucRxBuffer[0]!=0x04) && (ucRxBuffer[0]!=0x05) && (ucRxBuffer[0]!=0x06)) //数据头不对，则重新开始寻找数据头
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<8) {return;}//数据不满11个，则返回
	else
	{
		switch(ucRxBuffer[0])//判断数据是哪个激光测距仪返回的
		{
			case 0x04:	memcpy(&laserDis[0],&ucRxBuffer[4],2);break;//memcpy为编译器自带的内存拷贝函数，需引用"string.h"，将接收缓冲区的字符拷贝到数据结构体里面，从而实现数据的解析。
			case 0x05:	memcpy(&laserDis[1],&ucRxBuffer[4],2);break;
			case 0x06:	memcpy(&laserDis[2],&ucRxBuffer[4],2);break;
		}
		ucRxCnt=0;//清空缓存区
	}
}