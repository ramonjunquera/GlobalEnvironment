#-------------------------------------------------
#
# Project created by QtCreator 2017-04-05T09:55:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blinkArduino
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    RoJoArduino.cpp \
    bcm2835.c

HEADERS  += mainwindow.h \
    bcm2835.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

CONFIG += c++11
