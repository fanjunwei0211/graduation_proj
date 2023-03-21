#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NiMotionMotorSDK.h"

#define Nim

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->transOpenBtn->setEnabled(false);
    ui->transCloseBtn->setEnabled(false);
    carMode=1;//前进模式

//    int nAddr = 3;
//    int addrs[20];
//    int nCount = 0;
//    int rc = 0;
//    /*打开设备*/
//    rc = NiM_openDevice(0, "{\"DeviceName\": \"COM4\", \"Baudrate\" : 115200, \"Parity\" : \"None\", \"DataBits\" : 8, \"StopBits\" : 1}");
//    if (rc != 0) {
//        printf("Open device faild\r\n");
//        QMessageBox::warning(this, "WARNING", "打开设备失败", QMessageBox::Close);
//        return ;
//    }
//    /*扫描电机*/
//    rc = NiM_scanMotors(1, 10);
//    /*获取电机在线列表*/
//    rc = NiM_getOnlineMotors(addrs, &nCount);
//    /*打印在线电机列表*/
//    for (int i=0; i<nCount; i++)
//        printf("The Motor %d is online\r\n", addrs[i]);

//    /*设置速度模式*/
//    rc = NiM_changeWorkMode(nAddr,VELOCITY_MODE);
//    /*设置速度、加速度*/
//    rc = NiM_writeParam(nAddr,0x5B,4,600);
//    rc = NiM_writeParam(nAddr,0x5F,4,2000);
//    /*脱机*/
//    rc = NiM_powerOff(nAddr);
//    /*使能*/
//    rc = NiM_powerOn(nAddr);

//    printf("=======================================\r\n");
//    rc = NiM_moveVelocity(nAddr, 200);	//速度模式正转
//    NiM_writeParam(nAddr, 0x37, 4, 0x20000);
//    Sleep(2000);
//    rc = NiM_moveVelocity(nAddr, 350);	//速度模式正转
//    NiM_writeParam(nAddr, 0x37, 4, 0x00000);
//    Sleep(2000);
//    rc = NiM_moveVelocity(nAddr, 600);	//速度模式正转
//    NiM_writeParam(nAddr, 0x37, 4, 0x20000);
//    Sleep(2000);
//    rc = NiM_moveVelocity(nAddr, 300);	//速度模式正转
//    NiM_writeParam(nAddr, 0x37, 4, 0);
//    Sleep(2000);
//    rc = NiM_moveVelocity(nAddr, -200);	//速度模式反转
//    NiM_writeParam(nAddr, 0x37, 4, 0x20000);
//    Sleep(2000);
//    rc = NiM_moveVelocity(nAddr, -350);	//速度模式反转
//    NiM_writeParam(nAddr, 0x37, 4, 0);
//    Sleep(2000);
//    rc = NiM_moveVelocity(nAddr, -600);	//速度模式反转
//    NiM_writeParam(nAddr, 0x37, 4, 0x20000);
//    Sleep(2000);
//    rc = NiM_stop(nAddr);
//    rc = NiM_powerOff(nAddr);
//    Sleep(100);
//    NiM_closeDevice();
}

MainWindow::~MainWindow()
{
    delete ui;
}





void MainWindow::on_motorConnetBtn_clicked()
{
    if(ui->motorConnetBtn->text() == QString("连接电机")){
#ifdef Nim
        //SDK方式连接电机
        int addrs[20];
        int nCount = 0;
        int rc = 0;
        QString port;
        QString str,infostr;
        port = ui->motorBox->currentText();
        str = QString("{\"DeviceName\": \"%1\", \"Baudrate\" : 115200, \"Parity\" : \"None\", \"DataBits\" : 8, \"StopBits\" : 1}").arg(port);
        char* ch;
        QByteArray ba = str.toLatin1();
        ch=ba.data();
        /*打开设备*/
        rc = NiM_openDevice(0, ch);
        if (rc != 0) {
            printf("Open device faild\r\n");
            QMessageBox::warning(this, "WARNING", "打开设备失败", QMessageBox::Close);
            return ;
        }

        /*扫描电机*/
        NiM_scanMotors(1, 10);
        /*获取电机在线列表*/
        NiM_getOnlineMotors(addrs, &nCount);
        /*打印在线电机列表*/
        for (int i=0; i<nCount; i++)
            infostr += QString("The Motor %1 is online\r\n").arg(addrs[i]);
        ui->infoTBrowser->insertPlainText(infostr);
        NiM_changeWorkMode(1,VELOCITY_MODE);
        NiM_changeWorkMode(2,VELOCITY_MODE);
        NiM_changeWorkMode(3,VELOCITY_MODE);
        NiM_changeWorkMode(4,VELOCITY_MODE);
#else

        //port name 设置端口
        motorPort.setPortName(ui->motorBox->currentText());
        //baud rate 设置波特率
        motorPort.setBaudRate(115200);
        //parity 设置校验位
        motorPort.setParity(QSerialPort::NoParity);
        //data bits 设置数据位
        motorPort.setDataBits(QSerialPort::Data8);
        //stop bits 设置停止位
        motorPort.setStopBits(QSerialPort::OneStop);

        //port open 打开选择端口
        if(!motorPort.open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL,"提示","无法打开串口");
            return;
        }
#endif
        //失能串口选择
        ui->motorBox->setEnabled(false);
        ui->transOpenBtn->setEnabled(true);
        ui->transCloseBtn->setEnabled(true);
        ui->motorConnetBtn->setText(tr("断开电机"));

    }
    else{
#ifdef Nim
        NiM_stop(1);
        NiM_stop(2);
        NiM_stop(3);
        NiM_stop(4);
        NiM_powerOff(1);
        NiM_powerOff(2);
        NiM_powerOff(3);
        NiM_powerOff(4);
        NiM_closeDevice();
#else

        motorPort.close();
#endif
        //使能串口选择
        ui->motorBox->setEnabled(true);
        ui->motorConnetBtn->setText(tr("连接电机"));
    }
}




void MainWindow::on_motorScanBtn_clicked()
{
    ui->motorBox->clear();
    //通过QSerialPortInfo查找可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->motorBox->addItem(info.portName());
    }
}

void MainWindow::on_transOpenBtn_clicked()
{
    //三号电机DO1低电平 DO0高电平 0x10000->01 0000 0000 0000 0000
#ifdef Nim
    NiM_writeParam(3, 0x37, 4, 0x10000);
    NiM_powerOff(1);
    NiM_powerOff(2);
    NiM_powerOn(3);
    NiM_powerOn(4);
#else
    // 发送MODBUS RTU请求帧
    QByteArray requestFrame;
    requestFrame.resize(11);
    //03 10 00 37 00 02 04 00 01 00 00
    requestFrame[0] = 0x03;//ID
    requestFrame[1] = 0x10;//功能码
    requestFrame[2] = 0x00;//寄存器地址高位
    requestFrame[3] = 0x37;//寄存器地址低位
    requestFrame[4] = 0x00;//寄存器数量高位
    requestFrame[5] = 0x02;//寄存器数量低位
    requestFrame[6] = 0x04;//字节数
    //寄存器值
    requestFrame[7] = 0x00;
    requestFrame[8] = 0x02;
    requestFrame[9] = 0x00;
    requestFrame[10] = 0x00;
    int crc = calculateCRC16(requestFrame);
    requestFrame.append(0xff&crc);           // 添加CRC校验码低位
    requestFrame.append((0xff00&crc) >> 8);  // 添加CRC校验码高位
    motorPort.write(requestFrame);  // 发送请求帧
#endif
    carMode = 2;
}

void MainWindow::on_transCloseBtn_clicked()
{
    //三号电机DO1高电平 DO0低电平 0x20000->10 0000 0000 0000 0000
#ifdef Nim
    NiM_writeParam(3, 0x37, 4, 0x20000);
    NiM_powerOn(1);
    NiM_powerOn(2);
    NiM_powerOff(3);
    NiM_powerOff(4);
#else
    // 发送MODBUS RTU请求帧
    QByteArray requestFrame;
    requestFrame.resize(11);
    //03 10 00 37 00 02 04 00 02 00 00
    requestFrame[0] = 0x03;//ID
    requestFrame[1] = 0x10;//功能码
    requestFrame[2] = 0x00;//寄存器地址高位
    requestFrame[3] = 0x37;//寄存器地址低位
    requestFrame[4] = 0x00;//寄存器数量高位
    requestFrame[5] = 0x02;//寄存器数量低位
    requestFrame[6] = 0x04;//字节数
    //寄存器值
    requestFrame[7] = 0x00;
    requestFrame[8] = 0x01;
    requestFrame[9] = 0x00;
    requestFrame[10] = 0x00;
    int crc = calculateCRC16(requestFrame);
    requestFrame.append(0xff&crc);  // 添加CRC校验码低位
    requestFrame.append((0xff00&crc) >> 8);  // 添加CRC校验码高位
    motorPort.write(requestFrame);  // 发送请求帧
#endif
    carMode = 1;
}


void MainWindow::on_startBtn_clicked()
{
#ifdef Nim
    int speed;
    speed = ui->speedSpinbox->value();
    if(carMode == 1)  //前进模式
    {

        NiM_moveVelocity(1, speed);
        NiM_moveVelocity(2, -speed);
    }
    else if(carMode == 2) //平移模式
    {

        NiM_moveVelocity(3, speed);
        NiM_moveVelocity(4, -speed);
    }
#else

#endif
}


void MainWindow::on_stopBtn_clicked()
{
#ifdef Nim
    NiM_stop(1);
    NiM_stop(2);
    NiM_stop(3);
    NiM_stop(4);
    NiM_powerOff(1);
    NiM_powerOff(2);
    NiM_powerOff(3);
    NiM_powerOff(4);
#else

#endif
}

quint16 MainWindow::calculateCRC16(const QByteArray &data)
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

