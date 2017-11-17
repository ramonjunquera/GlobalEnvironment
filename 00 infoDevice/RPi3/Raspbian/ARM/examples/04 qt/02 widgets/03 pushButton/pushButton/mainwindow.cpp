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

void MainWindow::on_pushButton_Change_clicked()
{
    //Se ha pulsado el botón de cambiar

    //Asignamos un nuevo valor al texto del label
    ui->label_Hello->setText("Adiós");
}
