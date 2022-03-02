#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Arduino.h>
#include <RoJoVL53L1X.h>

//Importante:
//Este programa utiliza el puerto I2C
//Debe ser explícitamente activado desde "sudo raspi-config"

//Constantes globales
const byte idI2C[]={42,41}; //Identificadores I2C. No puede haber 2 iguales!!. Recomendable que el último sea el 0x29=41
const byte pinReset[]={18,23}; //Pines de reset

//Variables globales
static RoJoVL53L1X sensor[2]; //Array de sensores

//Función a la que llama el timer
void MainWindow::timerLauncher() {
  uint16_t value[2]; //Array de valores que leemos de los sensores
  for(byte i=0;i<2;i++) value[i]=sensor[i].read(); //Leemos los valores de todos los sensores
  ui->labelValue1->setText(QString::number(value[0])); //Mostramos el valor numérico del sensor 0
  ui->labelValue2->setText(QString::number(value[1])); //Mostramos el valor numérico del sensor 1
  for(byte i=0;i<2;i++) if(value[i]>1000) value[i]=1000; //Limitamos el valor máximo
  ui->slider1->setValue(value[0]); //Representamos el valor limitado en el slider 0
  ui->slider2->setValue(value[1]); //Representamos el valor limitado en el slider 1
}

//Constructor
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
  ui->setupUi(this);
  byte errorCode=RoJoVL53L1X::multiBegin(2,idI2C,pinReset,sensor);
  if(errorCode) Serial.println("Error inicializando sensores: "+String(static_cast<int>(errorCode)));
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
