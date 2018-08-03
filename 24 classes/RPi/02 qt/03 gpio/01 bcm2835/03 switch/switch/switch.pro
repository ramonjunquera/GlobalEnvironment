#-------------------------------------------------
#
# Project created by QtCreator 2017-04-05T12:39:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = switch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    RoJoArduino.cpp

HEADERS  += mainwindow.h \
    bcm2835.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

CONFIG += c++11
