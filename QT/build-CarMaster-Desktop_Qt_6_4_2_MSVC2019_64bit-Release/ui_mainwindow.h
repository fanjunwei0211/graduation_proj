/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *transOpenBtn;
    QPushButton *transCloseBtn;
    QComboBox *motorBox;
    QLabel *label;
    QPushButton *motorConnetBtn;
    QPushButton *laserConnetBtn;
    QComboBox *laserBox;
    QLabel *label_2;
    QTextBrowser *infoTBrowser;
    QPushButton *motorScanBtn;
    QPushButton *laserScanBtn;
    QSpinBox *speedSpinbox;
    QPushButton *startBtn;
    QLabel *label_3;
    QPushButton *stopBtn;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(676, 441);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        transOpenBtn = new QPushButton(centralwidget);
        transOpenBtn->setObjectName("transOpenBtn");
        transOpenBtn->setGeometry(QRect(10, 50, 71, 21));
        transCloseBtn = new QPushButton(centralwidget);
        transCloseBtn->setObjectName("transCloseBtn");
        transCloseBtn->setGeometry(QRect(90, 50, 71, 21));
        motorBox = new QComboBox(centralwidget);
        motorBox->setObjectName("motorBox");
        motorBox->setGeometry(QRect(70, 20, 62, 21));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 51, 21));
        motorConnetBtn = new QPushButton(centralwidget);
        motorConnetBtn->setObjectName("motorConnetBtn");
        motorConnetBtn->setGeometry(QRect(210, 20, 61, 21));
        laserConnetBtn = new QPushButton(centralwidget);
        laserConnetBtn->setObjectName("laserConnetBtn");
        laserConnetBtn->setGeometry(QRect(590, 20, 61, 21));
        laserBox = new QComboBox(centralwidget);
        laserBox->setObjectName("laserBox");
        laserBox->setGeometry(QRect(449, 20, 62, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(390, 20, 51, 21));
        infoTBrowser = new QTextBrowser(centralwidget);
        infoTBrowser->setObjectName("infoTBrowser");
        infoTBrowser->setGeometry(QRect(10, 280, 651, 121));
        motorScanBtn = new QPushButton(centralwidget);
        motorScanBtn->setObjectName("motorScanBtn");
        motorScanBtn->setGeometry(QRect(140, 20, 61, 21));
        laserScanBtn = new QPushButton(centralwidget);
        laserScanBtn->setObjectName("laserScanBtn");
        laserScanBtn->setGeometry(QRect(520, 20, 61, 21));
        speedSpinbox = new QSpinBox(centralwidget);
        speedSpinbox->setObjectName("speedSpinbox");
        speedSpinbox->setGeometry(QRect(60, 80, 81, 22));
        speedSpinbox->setMaximum(2000);
        startBtn = new QPushButton(centralwidget);
        startBtn->setObjectName("startBtn");
        startBtn->setGeometry(QRect(150, 80, 51, 21));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 80, 51, 21));
        stopBtn = new QPushButton(centralwidget);
        stopBtn->setObjectName("stopBtn");
        stopBtn->setGeometry(QRect(210, 80, 51, 21));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 676, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        transOpenBtn->setText(QCoreApplication::translate("MainWindow", "\345\267\246\345\217\263\345\271\263\347\247\273\345\274\200", nullptr));
        transCloseBtn->setText(QCoreApplication::translate("MainWindow", "\345\267\246\345\217\263\345\271\263\347\247\273\345\205\263", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\344\270\262\345\217\243", nullptr));
        motorConnetBtn->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\347\224\265\346\234\272", nullptr));
        laserConnetBtn->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\346\277\200\345\205\211", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\277\200\345\205\211\344\270\262\345\217\243", nullptr));
        motorScanBtn->setText(QCoreApplication::translate("MainWindow", "\346\211\253\346\217\217\347\224\265\346\234\272", nullptr));
        laserScanBtn->setText(QCoreApplication::translate("MainWindow", "\346\211\253\346\217\217\346\277\200\345\205\211", nullptr));
        startBtn->setText(QCoreApplication::translate("MainWindow", "\350\277\220\350\241\214", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\351\200\237\345\272\246", nullptr));
        stopBtn->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
