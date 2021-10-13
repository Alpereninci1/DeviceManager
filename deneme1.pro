QT      += core gui
QT      += serialport
QT      += serialbus
QT      += network
QT      += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = modbusmaster

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Update.cpp \
    device.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Update.h \
    device.h \
    mainwindow.h \
    mt_types.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    res/images/endoks.png \
    res/images/miltera.png

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/res.qrc
