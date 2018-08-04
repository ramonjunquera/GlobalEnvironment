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
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPlainTextEdit *plainTextEdit_Source;
    QPushButton *pushButton_Copy;
    QPlainTextEdit *plainTextEdit_Clone;
    QLabel *label_Source;
    QLabel *label_Clone;
    QLabel *label_Destination;
    QPlainTextEdit *plainTextEdit_Destination;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(273, 240);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        plainTextEdit_Source = new QPlainTextEdit(centralWidget);
        plainTextEdit_Source->setObjectName(QStringLiteral("plainTextEdit_Source"));
        plainTextEdit_Source->setGeometry(QRect(110, 10, 151, 31));
        plainTextEdit_Source->setFrameShape(QFrame::StyledPanel);
        plainTextEdit_Source->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextEdit_Source->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pushButton_Copy = new QPushButton(centralWidget);
        pushButton_Copy->setObjectName(QStringLiteral("pushButton_Copy"));
        pushButton_Copy->setGeometry(QRect(110, 90, 151, 31));
        plainTextEdit_Clone = new QPlainTextEdit(centralWidget);
        plainTextEdit_Clone->setObjectName(QStringLiteral("plainTextEdit_Clone"));
        plainTextEdit_Clone->setGeometry(QRect(110, 50, 151, 31));
        plainTextEdit_Clone->setFrameShape(QFrame::StyledPanel);
        plainTextEdit_Clone->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextEdit_Clone->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextEdit_Clone->setReadOnly(true);
        label_Source = new QLabel(centralWidget);
        label_Source->setObjectName(QStringLiteral("label_Source"));
        label_Source->setGeometry(QRect(10, 10, 91, 31));
        label_Clone = new QLabel(centralWidget);
        label_Clone->setObjectName(QStringLiteral("label_Clone"));
        label_Clone->setGeometry(QRect(10, 50, 91, 31));
        label_Destination = new QLabel(centralWidget);
        label_Destination->setObjectName(QStringLiteral("label_Destination"));
        label_Destination->setGeometry(QRect(10, 130, 91, 31));
        plainTextEdit_Destination = new QPlainTextEdit(centralWidget);
        plainTextEdit_Destination->setObjectName(QStringLiteral("plainTextEdit_Destination"));
        plainTextEdit_Destination->setGeometry(QRect(110, 130, 151, 31));
        plainTextEdit_Destination->setFrameShape(QFrame::StyledPanel);
        plainTextEdit_Destination->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextEdit_Destination->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextEdit_Destination->setReadOnly(true);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 273, 27));
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
        pushButton_Copy->setText(QApplication::translate("MainWindow", "Copy", 0));
        label_Source->setText(QApplication::translate("MainWindow", "Source", 0));
        label_Clone->setText(QApplication::translate("MainWindow", "Clone", 0));
        label_Destination->setText(QApplication::translate("MainWindow", "Destination", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
