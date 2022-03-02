//Atención. Esta aplicación utiliza un archivo de fuentes para la escritura de texto
//El archivo se encuentra en la carpeta data
//Puesto que la compilación y debug se realizan en otra carpeta que se crea dinámicamente
//debe ser copiada allí para que pueda ser leido por el ejecutable.

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Arduino.h>
#include <RoJoSSD1306.h>

static RoJoSSD1306 display; //Objeto de gestión del display
static RoJoSprite v(0); //Sprite monocromo del tamaño del display

//Inicialización de ventana
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow) {
  display.begin(); //Inicialización del display
  v.setSize(128,64); //Dimensionamos el sprite al tamaño del display: 128x64
  ui->setupUi(this); //Inicialización de ui
  on_horizontalSlider_valueChanged(0); //Inicializamos display con valor 0
}

//Queremos cerrar la ventana
MainWindow::~MainWindow() {
  v.end(); //Liberamos memoria del sprite
  display.clear(); //Borramos display
  delete ui;
}

//Ha cambiado el valor del slider
void MainWindow::on_horizontalSlider_valueChanged(int value) {
  ui->labelPercent->setText(QString::number(value)); //Actualizamos texto en la aplicación
  v.clear();
  v.rect(14,30,104,20,1); //Borde
  v.block(16,32,static_cast<int16_t>(value),16,1); //Barra
  v.printOver("/10x15d.fon",String(value),1,55,10); //Valor
  display.drawSprite(&v); //Mostramos el sprite
}
