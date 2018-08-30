#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>
#include <Arduino.h>

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

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    //La barra de desplazamiento ha cambiado su valor

    //Asignamos el mismo valor de la barra al PWM
    analogWrite(16,value);
}
