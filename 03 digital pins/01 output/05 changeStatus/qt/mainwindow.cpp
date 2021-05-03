#include <Arduino.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

//Declaración de constantes globales
const byte _pinLed=16; //Pin con el led

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Inicialización GPIO
    pinMode(_pinLed,OUTPUT);
    digitalWrite(_pinLed,LOW);
    //Inicialización UI
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_toggled(bool checked)
{
    //El botón ha cambiado de estado

    //Asignamos el mismo estado del botón al led
    digitalWrite(_pinLed,checked);
}
