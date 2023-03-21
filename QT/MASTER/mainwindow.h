#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void my_send_signals(bool);

private slots:
    void on_pBtn_send_clicked();

    void on_pBtn_scanSerial_clicked();

    void on_pBtn_openSerial_clicked(bool checked);
    //接收数据槽函数
    void receive_data(void);

    void on_pBtn_clearReceive_clicked();


    void on_cBox_autoSend_stateChanged(int arg1);


private:
    Ui::MainWindow *ui;
    //字符串转16进制
    QByteArray QString2Hex(QString str);
    //字符转16进制
    char ConvertHexChar(char ch);
    /*--------variable--------------------*/
    //串口全局类声明
    QSerialPort global_port;
    //自动发送定时器声明
    QTimer *timer;

};
#endif // MAINWINDOW_H
