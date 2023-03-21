QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    NiMotionMotorSDK.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



win32: LIBS += -L$$PWD/./ -lNiMotionMotorSDK

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/./NiMotionMotorSDK.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/./libNiMotionMotorSDK.a

win32: LIBS += -L$$PWD/./ -lModbusMaster

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/./ModbusMaster.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/./libModbusMaster.a

win32: LIBS += -L$$PWD/./ -lGlobal

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/./Global.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/./libGlobal.a
