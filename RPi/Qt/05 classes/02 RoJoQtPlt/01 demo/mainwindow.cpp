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
void MainWindow::on_pushButton_start_clicked() {
  //Mostraremos varios puntos
  plt->begin(); //Inicializamos plt con dimensiones por defecto
  //Creamos dos matrices de una sóla columna y tantas dilas como puntos
  //Contendrán las coordenadas x & y de cada punto
  RoJoFloatMatrix X(3,1),Y(3,1);
  //Le asignamos valores
  X.m[0][0]=-1;
  Y.m[0][0]=1;
  X.m[1][0]=0;
  Y.m[1][0]=1;
  X.m[2][0]=1;
  Y.m[2][0]=0;
  //Autocalcula escalas y dibuja ejes. No queremos que cada eje tenga
  //su propia escala
  plt->axis(false,&X,&Y);
  plt->scatter(&X,&Y,QColor(255,0,0)); //Muestra puntos en rojo
  //Asignamos valores nuevos
  X.m[0][0]=1;
  Y.m[0][0]=1;
  X.m[1][0]=-1;
  Y.m[1][0]=0;
  X.m[2][0]=1;
  Y.m[2][0]=0.5;
  //Sin variar la escala, dibujamos la nueva serie de puntos
  plt->scatter(&X,&Y,QColor(0,0,255));
  plt->show(); //Mostramos el resultado
}
