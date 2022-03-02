#ifndef MAINWINDOW_CPP
#define MAINWINDOW_CPP

#include <Arduino.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

const uint8_t pinLed=21;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow) { //Constructor
  ui->setupUi(this);
}

MainWindow::~MainWindow() { //Destructor
  delete ui;
}

//La barra de desplazamiento ha cambiado su valor
void MainWindow::on_horizontalSlider_valueChanged(int value) {
  analogWrite(pinLed,static_cast<byte>(value)); //Asignamos el mismo valor de la barra al PWM
}

#endif
