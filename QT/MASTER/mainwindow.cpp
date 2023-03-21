#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    //窗口最大化
//    setWindowState(Qt::WindowMaximized);

    //通过QSerialPortInfo查找可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        //将可用串口添加到端口显示框
        ui->cB_SerialChoose->addItem(info.portName());
    }
    //设置串口状态标签为红色 表示未连接状态
    ui->pBtn_openSerial->setText("打开串口");
    //statusBar 状态栏显示端口状态 未连接
    QString sm = "%1 CLOSED";
    QString status = sm.arg(ui->cB_SerialChoose->currentText());
    ui->statusbar->showMessage(status);
    ui->statusbar->setStyleSheet("color:red");
    //timer 自动发送
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,[=](){
        emit my_send_signals(true); //触发发送信号
    });

    //发送信号   发送槽函数
    connect(this,&MainWindow::my_send_signals,this,&MainWindow::on_pBtn_send_clicked);
    //receive data 串口数据接收完触发更新添加显示接收文本框
    connect(&global_port,&QSerialPort::readyRead,this,&MainWindow::receive_data);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pBtn_send_clicked()
{
    QString data = ui->receive_edit->toPlainText();
    QByteArray array;
    //自动换行
    if(ui->cBox_autoLine->checkState() == Qt::Checked ){
        data.append("\r\n");
    }
    //Hex复选框
    if(ui->cBox_hexSend->checkState() == Qt::Checked){
        array = QString2Hex(data);  //HEX 16进制
    }else{
        array = data.toLatin1();    //ASCII
    }
    if(global_port.isOpen())
        global_port.write(array);   //发送数据
}


void MainWindow::on_pBtn_scanSerial_clicked()
{
    ui->cB_SerialChoose->clear();
    //通过QSerialPortInfo查找可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cB_SerialChoose->addItem(info.portName());
    }
}

//字符串转16进制
QByteArray MainWindow::QString2Hex(QString str)
{
    QByteArray senddata;
        int hexdata,lowhexdata;
        int hexdatalen = 0;
        int len = str.length();

        senddata.resize(len/2);
        char lstr,hstr;

        for(int i=0; i<len; )
        {
            hstr=str[i].toLatin1();
            if(hstr == ' ')
            {
                i++;
                continue;
            }
            i++;
            if(i >= len)
                break;
            lstr = str[i].toLatin1();
            hexdata = ConvertHexChar(hstr);
            lowhexdata = ConvertHexChar(lstr);
            if((hexdata == 16) || (lowhexdata == 16))
                break;
            else
                hexdata = hexdata*16+lowhexdata;
            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }
        senddata.resize(hexdatalen);
        return senddata;
}

//字符转16进制
char MainWindow::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
            return ch-0x30;
        else if((ch >= 'A') && (ch <= 'F'))
            return ch-'A'+10;
        else if((ch >= 'a') && (ch <= 'f'))
            return ch-'a'+10;
        else return (-1);

}


void MainWindow::on_pBtn_openSerial_clicked(bool checked)
{
    if(ui->pBtn_openSerial->text() == QString("打开串口"))
    {
        //port name 设置端口
        global_port.setPortName(ui->cB_SerialChoose->currentText());
        //baud rate 设置波特率
        global_port.setBaudRate(ui->cB_baud->currentText().toInt());
        //parity 设置校验位
        switch (ui->cB_check->currentIndex()) {
        case 0:     //无校验
            global_port.setParity(QSerialPort::NoParity);
            break;
        case 1:     //偶校验
            global_port.setParity(QSerialPort::EvenParity);
            break;
        case 2:
            global_port.setParity(QSerialPort::OddParity);
            break;
        default:    //奇校验
            global_port.setParity(QSerialPort::OddParity);
            break;
        }

        //data bits 设置数据位
        switch (ui->cB_data->currentText().toInt()) {
        case 8:
            global_port.setDataBits(QSerialPort::Data8);
            break;
        case 7:
            global_port.setDataBits(QSerialPort::Data7);
            break;
        case 6:
            global_port.setDataBits(QSerialPort::Data6);
            break;
        default:
            global_port.setDataBits(QSerialPort::Data5);
            break;
        }

        //stop bits 设置停止位
        switch (ui->cB_stop->currentIndex()) {
        case 0:
            global_port.setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            global_port.setStopBits(QSerialPort::OneAndHalfStop);
            break;
        default:
            global_port.setStopBits(QSerialPort::TwoStop);
            break;
        }

        //port open 打开选择端口
        if(!global_port.open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL,"提示","无法打开串口");
            return;
        }
        //statusBar 状态栏显示端口状态
        QString sm = "%1 OPENED, %2, 8, NONE, 1";
        QString status = sm.arg(global_port.portName()).arg(global_port.baudRate());
        ui->statusbar->showMessage(status);
        ui->statusbar->setStyleSheet("color:green");
        ui->cB_SerialChoose->setEnabled(false);
        ui->cB_baud->setEnabled(false);
        ui->cB_check->setEnabled(false);
        ui->cB_data->setEnabled(false);
        ui->cB_stop->setEnabled(false);
        ui->pBtn_openSerial->setText(tr("关闭串口"));
    }
    else
    {
        global_port.close();
        //statusBar 状态栏显示端口状态
        QString sm = "%1 CLOSED";
        QString status = sm.arg(ui->cB_SerialChoose->currentText());
        ui->statusbar->showMessage(status);
        ui->statusbar->setStyleSheet("color:red");
        ui->cB_SerialChoose->setEnabled(true);
        ui->cB_baud->setEnabled(true);
        ui->cB_check->setEnabled(true);
        ui->cB_data->setEnabled(true);
        ui->cB_stop->setEnabled(true);
        ui->pBtn_openSerial->setText(tr("打开串口"));
        ui->cBox_autoLine->setChecked(0);
    }
}

void MainWindow::receive_data()
{
    QByteArray array = global_port.readAll();
    QString str_rev;

    if(ui->cBox_hexShow->checkState() == Qt::Checked){   //HEX 16进制
        if(ui->cBox_autoLine->checkState() == Qt::Checked){  //自动换行
            if(ui->cBox_time->checkState() == Qt::Checked){  //显示时间
                //获取当前系统时间
                QDateTime nowtime = QDateTime::currentDateTime();
                //时间转换为字符串格式
                str_rev = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
                //加上接收数据 转换为16进制并空格分开 接收数据换行
                str_rev += QString(array.toHex(' ').toUpper().append(' ')).append("\r\n");
            }else{
                str_rev = QString(array.toHex(' ').toUpper().append(' ')).append("\r\n");
            }
        }else{
            if(ui->cBox_time->checkState() == Qt::Checked){  //显示时间
                //获取当前系统时间
                QDateTime nowtime = QDateTime::currentDateTime();
                //时间转换为字符串格式
                str_rev = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
                //加上接收数据 转换为16进制并空格分开 接收数据换行
                str_rev += QString(array.toHex(' ').toUpper().append(' '));
            }else{
                str_rev = QString(array.toHex(' ').toUpper().append(' '));
            }
        }

    }else{
        if(ui->cBox_autoLine->checkState() == Qt::Checked){
            if(ui->cBox_time->checkState() == Qt::Checked){
                QDateTime nowtime = QDateTime::currentDateTime();
                str_rev = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
                str_rev += QString(array).append("\r\n");
            }else{
                str_rev = QString(array).append("\r\n");
            }
        }else{
            if(ui->cBox_time->checkState() == Qt::Checked){
                QDateTime nowtime = QDateTime::currentDateTime();
                str_rev = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
                str_rev += QString(array);
            }else{
                str_rev = QString(array);
            }

        }

    }
    ui->tB_receive->insertPlainText(str_rev);
}


void MainWindow::on_pBtn_clearReceive_clicked()
{
    ui->tB_receive->clear();
}


void MainWindow::on_cBox_autoSend_stateChanged(int arg1)
{
    if(arg1){
        timer->start(ui->sBox_time->value()); //启动定时器
    }else{
        timer->stop();  //停止定时器
    }

}



