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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton_toggle;
    QPushButton *pushButton_bulb;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(200, 92);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton_toggle = new QPushButton(centralWidget);
        pushButton_toggle->setObjectName(QStringLiteral("pushButton_toggle"));
        pushButton_toggle->setGeometry(QRect(20, 40, 81, 22));
        pushButton_bulb = new QPushButton(centralWidget);
        pushButton_bulb->setObjectName(QStringLiteral("pushButton_bulb"));
        pushButton_bulb->setGeometry(QRect(110, 10, 71, 71));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/icons/bulb-off-48.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_bulb->setIcon(icon);
        pushButton_bulb->setIconSize(QSize(48, 48));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        pushButton_toggle->setText(QApplication::translate("MainWindow", "toggle", 0));
        pushButton_bulb->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
