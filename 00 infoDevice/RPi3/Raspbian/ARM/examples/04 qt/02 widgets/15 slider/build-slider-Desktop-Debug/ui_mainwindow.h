/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QSlider *horizontalSlider;
    QSlider *verticalSlider;
    QLCDNumber *lcdNumber_horizontal;
    QLCDNumber *lcdNumber_vertical;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(203, 186);
        MainWindow->setTabShape(QTabWidget::Rounded);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(10, 160, 160, 16));
        horizontalSlider->setOrientation(Qt::Horizontal);
        verticalSlider = new QSlider(centralWidget);
        verticalSlider->setObjectName(QStringLiteral("verticalSlider"));
        verticalSlider->setGeometry(QRect(180, 10, 16, 160));
        verticalSlider->setOrientation(Qt::Vertical);
        lcdNumber_horizontal = new QLCDNumber(centralWidget);
        lcdNumber_horizontal->setObjectName(QStringLiteral("lcdNumber_horizontal"));
        lcdNumber_horizontal->setGeometry(QRect(10, 100, 61, 51));
        lcdNumber_horizontal->setDigitCount(2);
        lcdNumber_vertical = new QLCDNumber(centralWidget);
        lcdNumber_vertical->setObjectName(QStringLiteral("lcdNumber_vertical"));
        lcdNumber_vertical->setGeometry(QRect(110, 10, 61, 51));
        lcdNumber_vertical->setDigitCount(2);
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);
        QObject::connect(horizontalSlider, SIGNAL(valueChanged(int)), lcdNumber_horizontal, SLOT(display(int)));
        QObject::connect(verticalSlider, SIGNAL(valueChanged(int)), lcdNumber_vertical, SLOT(display(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
