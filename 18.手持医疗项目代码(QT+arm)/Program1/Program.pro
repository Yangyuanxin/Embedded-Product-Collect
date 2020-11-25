#-------------------------------------------------
#
# Project created by QtCreator 2014-08-19T14:48:59
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Program
TEMPLATE = app


SOURCES += main.cpp\
    doctordialog.cpp \
    patientdialog.cpp \
    floormapdialog.cpp \
    adddoctordialog.cpp \
    addpatientdialog.cpp \
    doctor.cpp \
    mainwindow.cpp \
    patient.cpp \
    widget.cpp \
    widget2.cpp \
    doctoradvice.cpp

HEADERS  += mainwindow.h \
    doctordialog.h \
    patientdialog.h \
    floormapdialog.h \
    adddoctordialog.h \
    addpatientdialog.h \
    doctor.h \
    patient.h \
    widget.h \
    widget2.h \
    doctoradvice.h

FORMS    += mainwindow.ui \
    doctordialog.ui \
    patientdialog.ui \
    floormapdialog.ui \
    adddoctordialog.ui \
    addpatientdialog.ui \
    widget.ui \
    widget2.ui \
    doctoradvice.ui

RESOURCES += \
    image.qrc
