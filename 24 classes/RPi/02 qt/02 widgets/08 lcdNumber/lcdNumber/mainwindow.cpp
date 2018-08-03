/*
 * Ejemplo de uso de objeto lcdNumber
 *
 *
 */

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

void MainWindow::on_pushButton_DEC_clicked()
{
    //Pulsado el botón de decrementar

    int value = ui->lcdNumber->intValue()-1;
    ui->lcdNumber->display(value);
}

void MainWindow::on_pushButton_INC_clicked()
{
    //Pulsado el botón de incrementar

    ui->lcdNumber->display(ui->lcdNumber->intValue()+1);
}
