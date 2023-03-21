#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_transOpenBtn_clicked();

    void on_motorConnetBtn_clicked();

    void on_motorScanBtn_clicked();

    void on_transCloseBtn_clicked();

    void on_startBtn_clicked();

    void on_stopBtn_clicked();

private:
    Ui::MainWindow *ui;
    int carMode; //0-->平移模式  1-->前进模式
    QSerialPort motorPort;
    quint16 calculateCRC16(const QByteArray &data);
};
#endif // MAINWINDOW_H
