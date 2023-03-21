#include "crc16.h"


quint16 calculateCRC16(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
        for (int i = 0; i < data.size(); i++)
        {
            crc ^= (quint8)data.at(i);
            for (int j = 0; j < 8; j++)
            {
                if (crc & 0x0001)
                {
                    crc >>= 1;
                    crc ^= 0xA001;
                }
                else
                {
                    crc >>= 1;
                }
            }
        }
    return crc;
//    return ((crc & 0xFF) << 8) | ((crc & 0xFF00) >> 8); //高四位字节和低四位字节位置互换
}
