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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QCheckBox *checkBox_Step2;
    QCheckBox *checkBox_Step3;
    QPushButton *pushButton_Step1;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(234, 105);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        checkBox_Step2 = new QCheckBox(centralWidget);
        checkBox_Step2->setObjectName(QStringLiteral("checkBox_Step2"));
        checkBox_Step2->setGeometry(QRect(100, 20, 72, 16));
        checkBox_Step3 = new QCheckBox(centralWidget);
        checkBox_Step3->setObjectName(QStringLiteral("checkBox_Step3"));
        checkBox_Step3->setGeometry(QRect(160, 20, 61, 16));
        pushButton_Step1 = new QPushButton(centralWidget);
        pushButton_Step1->setObjectName(QStringLiteral("pushButton_Step1"));
        pushButton_Step1->setGeometry(QRect(10, 10, 80, 31));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 234, 15));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(pushButton_Step1, SIGNAL(clicked()), checkBox_Step2, SLOT(toggle()));
        QObject::connect(checkBox_Step2, SIGNAL(toggled(bool)), checkBox_Step3, SLOT(setChecked(bool)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        checkBox_Step2->setText(QApplication::translate("MainWindow", "Step2", 0));
        checkBox_Step3->setText(QApplication::translate("MainWindow", "Step3", 0));
        pushButton_Step1->setText(QApplication::translate("MainWindow", "Step1", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
