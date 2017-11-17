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
    QPushButton *pushButton_status;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(182, 102);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label_bulb = new QLabel(centralWidget);
        label_bulb->setObjectName(QStringLiteral("label_bulb"));
        label_bulb->setGeometry(QRect(120, 20, 51, 61));
        label_bulb->setPixmap(QPixmap(QString::fromUtf8(":/icons/icons/bulb-off-48.png")));
        label_bulb->setAlignment(Qt::AlignCenter);
        pushButton_status = new QPushButton(centralWidget);
        pushButton_status->setObjectName(QStringLiteral("pushButton_status"));
        pushButton_status->setGeometry(QRect(20, 30, 80, 41));
        pushButton_status->setCheckable(true);
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label_bulb->setText(QString());
        pushButton_status->setText(QApplication::translate("MainWindow", "status", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
