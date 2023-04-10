/*
 * modbus_master_example.c
 *
 *  Created on: 2022年4月29日
 *      Author: hello
 */


#include "mbrtu_master.h"
#include "usart.h"
#include "tim.h"

#ifdef USE_RTOS

static void mutex_lock(void)
{

}

static void mutex_unlock(void)
{

}

#endif

static void timerStop(void)
{
    HAL_TIM_Base_Stop_IT(&htim14);
}

static void timerStart(void)
{
    __HAL_TIM_SET_COUNTER(&htim14, 0);
    HAL_TIM_Base_Start_IT(&htim14);
}

static void delayms(uint32_t nms)
{
    HAL_Delay(nms);
}

static uint32_t sendData(const void* buf, uint32_t len)
{
    if(HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, 100) != HAL_OK)
    {
        len = 0;
    }
    return len;
}

MBRTUMaterTypeDef MBRTUHandle =
{
    .delayms                      = delayms,
    .timerStart                   = timerStart,
    .timerStop                    = timerStop,
    .sendData                     = sendData,

#ifdef USE_RTOS  // 使用了RTOS那么需要实现互斥
    .lock                         = mutex_lock,
    .unlock                       = mutex_unlock,
#endif
};

#include <string.h>

int ret;
uint8_t ucBuf[10];
uint16_t usBuf[10];

//int main_example(void)
//{
//    // 定时器初始化,设置为3.5个字符的超时时间
//    // Timer_Init();

//    // 串口初始化,初始化波特率等
//    // UART_Init();

//    // 写单个线圈
//    ret = MBRTUMasterWriteSingleCoil(&MBRTUHandle, 1, 0, 1, 500);
//    printf(" write single coil %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    // 写单个寄存器
//    ret = MBRTUMasterWriteSingleRegister(&MBRTUHandle, 1, 0, 0XAABB, 500);
//    printf(" write single reg %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    // 写多个线圈
//    memset(ucBuf, 0X01, 10);
//    ret = MBRTUMasterWriteMultipleCoils(&MBRTUHandle, 1, 0, 10, ucBuf, 500);
//    printf(" write coils %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    // 写多个寄存器
//    memset(usBuf, 0XFF, 20);
//    ret = MBRTUMasterWriteMultipleRegisters(&MBRTUHandle, 1, 0, 10, usBuf, 500);
//    printf(" write regs %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    // 读线圈
//    MBRTUMasterReadCoils(&MBRTUHandle, 1, 0, 10, 500, ucBuf);
//    printf(" read coils %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    // 读离散量输入
//    MBRTUMasterReadDiscreteInputs(&MBRTUHandle, 1, 0, 10, 500, ucBuf);
//    printf(" read discs %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    // 读保持寄存器
//    MBRTUMasterReadHoldingRegisters(&MBRTUHandle, 1, 0, 10, 500, usBuf);
//    printf(" read hold regs %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    // 读输入寄存器
//    MBRTUMasterReadInputRegisters(&MBRTUHandle, 1, 0, 10, 500, usBuf);
//    printf(" read input regs %s. \r\n", ret < 0 ? "failed" : "ok");
//    HAL_Delay(100);

//    return 0;
//}
