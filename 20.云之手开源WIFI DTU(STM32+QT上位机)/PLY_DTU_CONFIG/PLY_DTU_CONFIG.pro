#-------------------------------------------------
#
# Project created by QtCreator 2019-12-21T13:47:11
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PLY_DTU_CONFIG
TEMPLATE = app
RC_FILE += app.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    dtu_device_mode.cpp \
    esp8266_mode.cpp

HEADERS  += mainwindow.h \
    gobal_para.h

FORMS    += mainwindow.ui

RESOURCES +=
