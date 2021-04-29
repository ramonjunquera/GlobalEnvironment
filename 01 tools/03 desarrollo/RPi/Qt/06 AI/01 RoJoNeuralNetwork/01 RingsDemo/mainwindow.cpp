/*
  Autor: Ramón Junquera
  Fecha: 20200911
  Tema: Redes Neuronales
  Objetivo: Demo

  Descripción:
    Haciendo uso de las distintas librerías, generaremos un par
    de anillos concéntricos y entrenaremos una red neuronal para
    que sea capaz de distinguir uno de otro.
    Se representa gráficamente el resultado.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

//Constructor
MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

//Destructor
MainWindow::~MainWindow() {
  delete ui;
}

//Pulsado botón start
//Mostraremos dos anillos con distintos colores
void MainWindow::on_pushButton_start_clicked() {
  //Definición de variables globales
  RoJoQtPlt plt(ui->label_plt); //Objeto de representación de gráfica de funciones
  RoJoFloatMatrixQt R0,R1; //Matrices de coordenadas de cada anillo
  uint16_t samples=200; //Número de puntos de cada anillo
  RoJoFloatMatrixQt topology(3,1); //Definición de la topología de la red neuronal
  RoJoNeuralNetworkQt nn; //NeuralNetwork Object
  RoJoFloatMatrixQt X; //Matriz de coordenadas de todos los puntos, para entrenamiento
  RoJoFloatMatrixQt Z; //Matriz que guardará los valores de salida
  RoJoFloatMatrixQt Y(samples*2,1); //Matriz de valores correctos para el entrenamiento de la red
  RoJoFloatMatrixQt cXb,cYb; //Guardarán coordenadas de mapa de background
  RoJoFloatMatrixQt Vb; //Valor de cada pixel de mapa de background
  RoJoFloatMatrixQt meanCost; //Promedio de los resultados de la función de coste
  uint8_t resolution=50; //Número de rectángulos por eje para mapa de background
  float error=1; //Resultado de la función de coste
  uint16_t epoch=0; //Contador de ciclos

  //Inicialización general
  plt.begin(); //Inicializamos plt con dimensiones por defecto (400x400)
  //Definición de topología. 3 capas
  topology.m[0][0]=2; //Datos de entrada: 2 = coordenadas del punto
  topology.m[1][0]=4; //Nodos de la capa oculta
  topology.m[2][0]=1; //Datos de salida: a qué anillo pertenece?

  //Inicialización para cada ejemplo
  RoJoMakeCirclesQt circles; //Objeto para generar anillos
  circles.get(&R0,samples,0.5,0.1); //Generamos puntos de anillo 0 (interior,radio=0.5)
  circles.get(&R1,samples,1,0.1); //Generamos puntos de anillo 1 (exterior,radio=1)
  plt.axis(false,&R1); //Autocalcula escalas y dibuja ejes con datos de anillo exterior
  //Combinamos las coordenadas de ambos anillos en una sóla matriz para la red neuronal
  //Serán los valores de entrada para el entrenamiento de la red
  X.glueRows(&R0,&R1); //Pegamos las filas de ambos anillos en una sola matriz
  //Asignamos el valor correcto para cada anillo
  //Al unir las dos matrices en una se respeta el orden de los parámetros
  for(uint16_t i=0;i<samples;i++) {
    Y.m[i][0]=0; //Anillo 0 en la primera mitad
    Y.m[i+samples][0]=1; //Anillo 1 en la segunda mitad
  }
  //Desordenaremos los datos
  //Creamos una lista desordenada con tantos elementos como filas tenga la matriz Y
  //La lista contiene una secuencia que comienza en 0 y está desordenada
  uint16_t *sList=Y.scrambleList();
  //Desordenamos la matrices de coordenadas y valores teniendo en cuenta la lista
  X.scramble(sList);
  Y.scramble(sList);
  delete[] sList; //Ya no necesitamos la lista desordenada
  //Inicializamos red con topología, función de activación y función de coste
  nn.begin(&topology,sigmoidF,cuadErrorF);
  //Inicializamos los parámetros de aprendizaje:
  //- Derivada de la función de activación
  //- Función de coste/error
  //- Derivada de la función de coste/error
  //- learning rate
  nn.beginTrain(sigmoidD,cuadErrorD,0.025); //Init train
  float xMin,yMin,xMax,yMax; //Guardarán los límites de visualización
  plt.getLimitsX(&xMin,&xMax); //Obtenemos límites horizontales
  plt.getLimitsY(&yMin,&yMax); //Obtenemos límites verticales
  cXb.linspace(xMin,xMax,resolution); //Calculamos coordenadas X de pixels de mapa de background
  cYb.linspace(yMin,yMax,resolution); //Calculamos coordenadas Y de pixels de mapa de background
  //Definimos los límites de los valores del mapa de background
  //Así no tendremos que calcularlo en cada ciclo
  //Realmente no es necesario, porque coinciden con los valores por defecto
  //[0,1] son los límites de la función sigmoide
  plt.meshMin=0;
  plt.meshMax=1;
  error=1; //Error superior al límite mínimo rovoca que comience a trabajar
  epoch=0; //Reseteamos el número de ciclos

  //Ciclo de cálculo
  while(error>0.01) { //Mientras no tengamos un valor reducido...
    epoch++;
    if(epoch%50==0) { //Cada 50 ciclos...
      //Calculamos el error tomando los valores de salida del último ciclo
      nn.cost(&Z,&Y,&meanCost);
      //Como sólo tenemos un parámetro de salida, la matriz meanCost
      //Sólo tendrá una columna (y una fila).
      //Tomaremos el valor como el error del estado actual
      error=meanCost.m[0][0]; //Lo anotamos para poder salir del bucle
      qDebug() << "ciclo:" << epoch << ",error=" << error;
      nn.mesh(&cXb,&cYb,&Vb); //Calculamos los valores del mapa de background
      //Dibujamos el mapa de backgroud y pasamos como parámetros:
      //- cXb : la lista de coordenadas horizontales
      //- cYb : La lista de coordenadas verticales
      //- Vb : La matriz de valores
      //- meshRWB : La función que convierte los valores en colores
      //- autoLimit: debe calcular automáticamente los límites? NO
      plt.mesh(&cXb,&cYb,&Vb,meshRWB,false);
      plt.scatter(&R0,QColor(255,0,0)); //Dibujamos anillo 0 en rojo
      plt.scatter(&R1); //Dibujamos anillo 1 en azul
      plt.show(); //Mostramos el resultado
    } else { //Si no toca mostrar resultados...
      nn.train(&X,&Y,&Z); //Entrenamos con normalidad
    }
  }
}
