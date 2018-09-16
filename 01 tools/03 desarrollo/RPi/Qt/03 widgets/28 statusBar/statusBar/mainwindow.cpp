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

void MainWindow::on_pushButton0_clicked()
{
    //Se ha pulsado el botón para mensaje permanente
    ui->statusBar->showMessage("Este es un mensaje permanenente");
}

void MainWindow::on_pushButtonDelete_clicked()
{
    //Se ha pulsado el botón para borrar el mensaje
    ui->statusBar->clearMessage();
}

void MainWindow::on_pushButton2_clicked()
{
    //Se ha pulsado el botón para mensaje de 2 segundos

    //Fijamos un límite de tiempo para mantener visible el mensaje
    //El tiempo de mide en milisegundos
    ui->statusBar->showMessage("Este mensaje dura 2 segundos",2000);
}


