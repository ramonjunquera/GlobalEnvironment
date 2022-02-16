/*
 * Tema: Uso de librería Arduino.h para gestiones de pines digitales
 * Fecha: 20220215
 * Autor: Ramón Junquera
 *
 * La aplicación consiste en un botón que cambia el estado de un led y además lo muestra
 * con una imagen.
 *
 * Sólamente hay que dejar en la carpeta del proyecto el archivo Arduino.h
 * No es obligatorios incluirlo en las secciones de Headers y Sources, pero si se
 * hace, como es este ejemplo, se reconocerán las funciones incluidas y será más
 * fácil utilizarlas.
 *
 * Además hay que añadir la línea
 *   #include <Arduino.h>
 * al principio del código que la vaya a utilizar. En este caso mainwindow.cpp
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <Arduino.h>
using namespace std;

//Definición de pines
const int pinLed=21;

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) { //Constructor
  pinMode(pinLed,OUTPUT); //Definimos el pin como salida
  ui->setupUi(this);
}

MainWindow::~MainWindow() { //Destructor
  ArduinoEnd(); //Hemos terminado de utilizar la librería
  delete ui;
}

//Pulsado el botón toggle
void MainWindow::on_pushButton_toggle_clicked() {
  static bool status = false;
  status=!status; //Cambiamos el estado del led
  //Leemos la imagen del archivo de recursos en función del estado
  QPixmap image=QPixmap(status?":/icons/icons/bulb-on-48.png":":/icons/icons/bulb-off-48.png");
  ui->label_bulb->setPixmap(image); //Asignamos la imagen al label
  digitalWrite(pinLed,status?HIGH:LOW); //Asignamos estado al pin del led
}
