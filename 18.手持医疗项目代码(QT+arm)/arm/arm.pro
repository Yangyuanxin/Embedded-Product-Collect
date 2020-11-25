#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T15:27:14
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = arm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    doctordialog.cpp \
    patientdialog.cpp \
    floormapdialog.cpp \
    bsdialog.cpp \
    bpdialog.cpp \
    calculatordialog.cpp \
    doctoradvice.cpp

HEADERS  += mainwindow.h \
    doctordialog.h \
    patientdialog.h \
    floormapdialog.h \
    bsdialog.h \
    bpdialog.h \
    calculatordialog.h \
    doctoradvice.h

FORMS    += mainwindow.ui \
    doctordialog.ui \
    patientdialog.ui \
    floormapdialog.ui \
    bsdialog.ui \
    bpdialog.ui \
    calculatordialog.ui \
    doctoradvice.ui

RESOURCES += \
    image.qrc
