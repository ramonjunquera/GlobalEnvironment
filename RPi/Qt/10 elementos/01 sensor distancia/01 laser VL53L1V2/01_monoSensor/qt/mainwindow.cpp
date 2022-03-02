#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Arduino.h>
#include <RoJoVL53L1X.h>

//Importante:
//Este programa utiliza el puerto I2C
//Debe ser explícitamente activado desde "sudo raspi-config"

//Variables globales
static RoJoVL53L1X sensor;

//Función a la que llama el timer
void MainWindow::timerLauncher() {
  uint16_t value=sensor.read(); //Leemos el valor de la distancia
  ui->label->setText(QString::number(value)); //Mostramos el valor numérico en el label
  if(value>2000) value=2000; //Limitamos el valor leido
  ui->horizontalSlider->setValue(value); //Representamos el valor limitado en el Slider
}

//Constructor
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
  ui->setupUi(this);
  if (!sensor.begin()) Serial.println("Error inicializando el sensor"); //Informamos
  myTimer = new QTimer(this); //Creamos una nueva instancia del timer
  connect(myTimer,SIGNAL(timeout()),this,SLOT(timerLauncher())); //Le asignamos la función a la que llamar
}

//Destructor
MainWindow::~MainWindow() {
  myTimer->stop();
  delete ui;
}

//El botón start/stop ha cambiado de estado
void MainWindow::on_pushButtonStartStop_toggled(bool checked) {
  if(checked) myTimer->start(300); //Arrancamos el timer
  else myTimer->stop(); //Paramos el timer
}
