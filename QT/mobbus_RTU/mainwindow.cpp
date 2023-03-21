#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crc16.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    QByteArray byteAry3 = QByteArray::fromHex("031000510001020007");
//    qDebug() << "byte array 3 is:" << byteAry3;		// ret 0xAB CC DD..
//    qDebug()<<"CRC--" << calculateCRC16(byteAry3);


//    // 打开串口
//    QSerialPort serialPort;
//    serialPort.setPortName("COM5");  // 设置串口名称
//    serialPort.setBaudRate(QSerialPort::Baud115200);  // 设置波特率
//    serialPort.setDataBits(QSerialPort::Data8);  // 设置数据位
//    serialPort.setParity(QSerialPort::NoParity);  // 设置校验位
//    serialPort.setStopBits(QSerialPort::OneStop);  // 设置停止位
//    serialPort.setFlowControl(QSerialPort::NoFlowControl);  // 设置流控制

//    if (!serialPort.open(QIODevice::ReadWrite)) {  // 打开串口
//        qDebug() << "串口打开失败！";
//        return ;
//    }

    // 发送MODBUS RTU请求帧
    QByteArray requestFrame;
    requestFrame.resize(9);
    //03 10 00 51 00 01 02 00 07 F3 73
    requestFrame[0] = 0x03;
    requestFrame[1] = 0x10;
    requestFrame[2] = 0x00;
    requestFrame[3] = 0x51;
    requestFrame[4] = 0x00;
    requestFrame[5] = 0x01;
    requestFrame[6] = 0x02;
    requestFrame[7] = 0x00;
    requestFrame[8] = 0x07;
    int crc = calculateCRC16(requestFrame);
    requestFrame.append(0xff&crc);  // 添加CRC校验码低位
    requestFrame.append((0xff00&crc) >> 8);  // 添加CRC校验码高位
    qDebug()<< requestFrame.toHex().toUpper();
//    serialPort.write(requestFrame);  // 发送请求帧

//    // 接收MODBUS RTU响应帧
//    if (serialPort.waitForReadyRead(5000)) {  // 等待接收数据
//        QByteArray responseFrame = serialPort.readAll();  // 读取响应帧
//        qDebug() << "接收到数据：" << responseFrame.toHex().toUpper();

//        // 解析响应帧
//        if (responseFrame.length() >= 5) {  // 响应帧至少包含5个字节
//            int deviceAddress = responseFrame[0];  // 设备地址
//            int functionCode = responseFrame[1];  // 功能码
//            int byteCount = responseFrame[2];  // 数据字节数
//            QByteArray data = responseFrame.mid(3, byteCount);  // 数据域
//            int crc = calculateCRC16(responseFrame);  // 计算CRC校验码
//            int receivedCrc = responseFrame.right(2).toHex().toInt(nullptr, 16);  // 接收到的CRC校验码
//            if (crc == receivedCrc) {  // 校验CRC校验码
//                qDebug() << "设备地址：" << deviceAddress;
//                qDebug() << "功能码：" <<functionCode;
//                qDebug() << "数据字节数：" << byteCount;
//                qDebug() << "数据域：" << data.toHex().toUpper();
//            } else {
//                qDebug() << "CRC校验失败！";
//            }
//        } else {
//            qDebug() << "响应帧格式错误！";
//        }
//    } else {
//        qDebug() << "接收数据超时！";
//    }

//    // 关闭串口
//    serialPort.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

