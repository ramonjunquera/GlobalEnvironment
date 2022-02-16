#-------------------------------------------------
#
# Project created by QtCreator 2018-08-30T19:39:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pwm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    Arduino.h

FORMS    += mainwindow.ui

CONFIG += c++14
INCLUDEPATH += /home/pi/prg
