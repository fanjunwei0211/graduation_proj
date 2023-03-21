#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    //字符串转16进制
    QByteArray QString2Hex(QString str);
    //字符转16进制
    char ConvertHexChar(char ch);
    /*--------variable--------------------*/
    //串口全局类声明
    QSerialPort Motor_port,Lazer_port;
    //自动发送定时器声明
    QTimer *timer;
};
#endif // WIDGET_H
