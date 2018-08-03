#-------------------------------------------------
#
# Project created by QtCreator 2017-04-04T12:22:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blink
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bcm2835.c

HEADERS  += mainwindow.h \
    bcm2835.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
