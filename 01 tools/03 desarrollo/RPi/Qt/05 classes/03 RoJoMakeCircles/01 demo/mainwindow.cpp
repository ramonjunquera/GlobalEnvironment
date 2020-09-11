#include "mainwindow.h"
#include "ui_mainwindow.h"

//Constructor
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
  ui->setupUi(this);
  plt=new RoJoQtPlt(ui->label_plt); //Inicializamos objeto plt con el label
}

//Destructor
MainWindow::~MainWindow() {
  delete ui;
  delete plt; //Borramos objeto plt para evitar memory leaks
}

//Pulsado botón start
//Mostraremos dos anillos con distintos colores
void MainWindow::on_pushButton_start_clicked() {
  uint32_t samples=200; //Número de puntos de cada anillo
  plt->begin(); //Inicializamos plt con dimensiones por defecto
  RoJoMakeCircles circles; //Objeto para generar anillos
  RoJoFloatMatrix X,Y; //Matrices para coordenadas de cada anillo
  circles.get(samples,&X,&Y,2,0.25); //Generación de anillo 0
  plt->axis(false,&X,&Y); //Autocalcula escalas y dibuja ejes
  plt->scatter(&X,&Y,QColor(255,0,0)); //Anillo 0 en rojo
  circles.get(samples,&X,&Y,1,0.25); //Generación de anillo 1
  plt->scatter(&X,&Y,QColor(0,0,255)); //Anillo 1 en azul
  plt->show(); //Mostramos el resultado
}
