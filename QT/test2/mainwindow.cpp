#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "NiMotionMotorSDK.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    int nAddr = 1;
    int addrs[20];
    int nCount = 0;
    int rc = 0;
    /*打开设备*/
    rc = NiM_openDevice(0, "{\"DeviceName\": \"COM4\", \"Baudrate\" : 115200, \"Parity\" : \"None\", \"DataBits\" : 8, \"StopBits\" : 1}");
    if (rc != 0) {
        printf("Open device faild\r\n");
        QMessageBox::warning(this, "WARNING", "打开设备失败", QMessageBox::Close);
        return ;
    }
    /*扫描电机*/
    rc = NiM_scanMotors(1, 10);
    /*获取电机在线列表*/
    rc = NiM_getOnlineMotors(addrs, &nCount);
    /*打印在线电机列表*/
    for (int i=0; i<nCount; i++)
        printf("The Motor %d is online\r\n", addrs[i]);

    /*设置速度模式*/
    rc = NiM_changeWorkMode(nAddr,VELOCITY_MODE);
    /*设置速度、加速度*/
    rc = NiM_writeParam(nAddr,0x5B,4,600);
    rc = NiM_writeParam(nAddr,0x5F,4,2000);
    /*脱机*/
    rc = NiM_powerOff(nAddr);
    /*使能*/
    rc = NiM_powerOn(nAddr);

    printf("=======================================\r\n");
    rc = NiM_moveVelocity(nAddr, 200);	//速度模式正转
    Sleep(2000);
    rc = NiM_moveVelocity(nAddr, 350);	//速度模式正转
    Sleep(2000);
    rc = NiM_moveVelocity(nAddr, 600);	//速度模式正转
    Sleep(2000);
    rc = NiM_moveVelocity(nAddr, 300);	//速度模式正转
    Sleep(2000);
    rc = NiM_moveVelocity(nAddr, -200);	//速度模式反转
    Sleep(2000);
    rc = NiM_moveVelocity(nAddr, -350);	//速度模式反转
    Sleep(2000);
    rc = NiM_moveVelocity(nAddr, -600);	//速度模式反转
    Sleep(2000);
    rc = NiM_stop(nAddr);
    rc = NiM_powerOff(nAddr);
    Sleep(100);
    NiM_closeDevice();
}

