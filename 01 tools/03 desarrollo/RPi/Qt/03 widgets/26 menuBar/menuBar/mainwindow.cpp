#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action1_triggered()
{
    //Se ha pulsado la opción 1

    //Mostramos en el QLabel la opción
    ui->label->setText("1");
}

void MainWindow::on_action2_triggered()
{
    //Se ha pulsado la opción 2

    //Mostramos en el QLabel la opción
    ui->label->setText("2");
}

void MainWindow::on_action3_triggered()
{
    //Se ha pulsado la opción 3

    //Mostramos en el QLabel la opción
    ui->label->setText("3");
}

void MainWindow::on_actionExit_triggered()
{
    //Se ha pulsado la opción Exit

    //Salimos de la aplicación
    QApplication::exit();
}
