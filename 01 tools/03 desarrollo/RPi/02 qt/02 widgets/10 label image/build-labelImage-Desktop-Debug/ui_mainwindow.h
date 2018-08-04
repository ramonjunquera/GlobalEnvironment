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
    QLabel *label;
    QPushButton *pushButton_Image1;
    QPushButton *pushButton_Image2;
    QPushButton *pushButton_Image3;
    QPushButton *pushButton_Text;
    QPushButton *pushButton_empty;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(172, 162);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(80, 10, 81, 71));
        label->setAlignment(Qt::AlignCenter);
        pushButton_Image1 = new QPushButton(centralWidget);
        pushButton_Image1->setObjectName(QStringLiteral("pushButton_Image1"));
        pushButton_Image1->setGeometry(QRect(10, 10, 61, 16));
        pushButton_Image2 = new QPushButton(centralWidget);
        pushButton_Image2->setObjectName(QStringLiteral("pushButton_Image2"));
        pushButton_Image2->setGeometry(QRect(10, 30, 61, 16));
        pushButton_Image3 = new QPushButton(centralWidget);
        pushButton_Image3->setObjectName(QStringLiteral("pushButton_Image3"));
        pushButton_Image3->setGeometry(QRect(10, 50, 61, 16));
        pushButton_Text = new QPushButton(centralWidget);
        pushButton_Text->setObjectName(QStringLiteral("pushButton_Text"));
        pushButton_Text->setGeometry(QRect(10, 70, 61, 16));
        pushButton_empty = new QPushButton(centralWidget);
        pushButton_empty->setObjectName(QStringLiteral("pushButton_empty"));
        pushButton_empty->setGeometry(QRect(10, 90, 61, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 172, 15));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QApplication::translate("MainWindow", "Hello", 0));
        pushButton_Image1->setText(QApplication::translate("MainWindow", "image1", 0));
        pushButton_Image2->setText(QApplication::translate("MainWindow", "image2", 0));
        pushButton_Image3->setText(QApplication::translate("MainWindow", "image3", 0));
        pushButton_Text->setText(QApplication::translate("MainWindow", "text", 0));
        pushButton_empty->setText(QApplication::translate("MainWindow", "empty", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
