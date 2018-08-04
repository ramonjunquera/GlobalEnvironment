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

void MainWindow::on_checkBox_Source_clicked()
{
    //Ha cambiado el estado de checkBox_Source

    //Anotamos el estado de checkBox_Source
    bool status = ui->checkBox_Source->isChecked();
    //Lo aplicampos a checkBox_Destination
    ui->checkBox_Destination->setChecked(status);
    //Cambiamos el texto de la etiqueta en función del estado
    ui->label_OnOff->setText(status?"ON":"OFF");
}
