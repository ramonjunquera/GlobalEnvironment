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
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label_bulb;
    QPushButton *pushButton_Start;
    QPushButton *pushButton_End;
    QPushButton *pushButton_Once;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(175, 85);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label_bulb = new QLabel(centralWidget);
        label_bulb->setObjectName(QStringLiteral("label_bulb"));
        label_bulb->setGeometry(QRect(100, 10, 61, 61));
        label_bulb->setPixmap(QPixmap(QString::fromUtf8(":/icons/icons/bulb-off-48.png")));
        label_bulb->setAlignment(Qt::AlignCenter);
        pushButton_Start = new QPushButton(centralWidget);
        pushButton_Start->setObjectName(QStringLiteral("pushButton_Start"));
        pushButton_Start->setGeometry(QRect(10, 10, 41, 31));
        pushButton_End = new QPushButton(centralWidget);
        pushButton_End->setObjectName(QStringLiteral("pushButton_End"));
        pushButton_End->setEnabled(false);
        pushButton_End->setGeometry(QRect(50, 10, 41, 31));
        pushButton_Once = new QPushButton(centralWidget);
        pushButton_Once->setObjectName(QStringLiteral("pushButton_Once"));
        pushButton_Once->setGeometry(QRect(10, 40, 81, 31));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label_bulb->setText(QString());
        pushButton_Start->setText(QApplication::translate("MainWindow", "Start", 0));
        pushButton_End->setText(QApplication::translate("MainWindow", "End", 0));
        pushButton_Once->setText(QApplication::translate("MainWindow", "Once", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
