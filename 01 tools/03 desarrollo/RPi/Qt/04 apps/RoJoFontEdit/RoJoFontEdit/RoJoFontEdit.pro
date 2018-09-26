#-------------------------------------------------
#
# Project created by QtCreator 2018-09-24T13:51:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RoJoFontEdit
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    datachar.cpp \
    datafont.cpp \
    qlabelmouse.cpp

HEADERS  += mainwindow.h \
    datachar.h \
    typedef.h \
    datafont.h \
    qlabelmouse.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc
