QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    creatfile.cpp \
    funcwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mcuinfo.cpp \
    otherwidget.cpp \
    pininfowidget.cpp \
    pinsmap.cpp \
    pinswidget.cpp \
    projwidget.cpp \
    stm32pinsmap.cpp \
    xmlfile.cpp

HEADERS += \
    creatfile.h \
    funcwidget.h \
    mainwindow.h \
    mcuinfo.h \
    otherwidget.h \
    pininfowidget.h \
    pinsmap.h \
    pinswidget.h \
    projwidget.h \
    stm32pinsmap.h \
    xmlfile.h

FORMS += \
    funcwidget.ui \
    mainwindow.ui \
    otherwidget.ui \
    pininfowidget.ui \
    pinswidget.ui \
    projwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TARGET=pinout-generator
RC_ICONS = RTduino.ico
