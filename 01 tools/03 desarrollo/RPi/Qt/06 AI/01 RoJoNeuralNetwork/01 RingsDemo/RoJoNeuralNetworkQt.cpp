/*
 * Autor: Ramón Junquera
 * Fecha: 20200911
 * Tema: Gestión de redes neuronales
 */

#include "RoJoNeuralNetworkQt.h"

//Definición de algunas funciones de activación
float sigmoidF(float x) { //Sigmoidea
  return 1/(1+exp(-x));
}
float sigmoidD(float x) { //Derivada de sigmoidea
  return x*(1-x);
}
float reluF(float x) { //ReLU
  if(x<=0) return 0;
  return x;
}
float reluD(float x) { //Derivada de ReLU
  if(x<=0) return 0;
  return 1;
}

//Definición de error cuadrático como función de coste
float cuadErrorF(float valueRight,float valueGot) {
  float t=valueRight-valueGot;
  return t*t;
}
float cuadErrorD(float valueRight,float valueGot) { //Derivada
  return valueRight-valueGot;
}


//class RoJoNeuralLayerQt--------------------------------------------------------


//Inicialización y dimensionamiento
//Parámetros:
//- inNodes : Nodos de entrada
//- outNodes : Nodos de salida/internos
//Devuelve true si lo consigue
bool RoJoNeuralLayerQt::begin(uint16_t inNodes,uint16_t outNodes) {
  if(inNodes==0 || outNodes==0) return false;
  //Guardamos parámetros
  this->inNodes=inNodes;
  this->outNodes=outNodes;
  //Dimensionamos las matrices
  _W.redim(inNodes,outNodes);
  _b.redim(1,outNodes);
  //Las inicializamos con valores aleatorios normalizados
  _W.fillRand();
  _b.fillRand();
  return true; //Todo Ok
}

void RoJoNeuralLayerQt::end() {
  _b.end();
  _W.end();
  sumW.end();
  outValues.end();
}

//Destructor
RoJoNeuralLayerQt::~RoJoNeuralLayerQt() {
  end();
}

//Calcula los valores de salida (con función de activación)
//Parámetros:
//- inValues: matrix de datos de entrada de tamaño (samples,outNodes)
//Devuelve true si lo consigue
bool RoJoNeuralLayerQt::calc(float (*actF)(float)) {
  if(inValues->rows()==0 || inValues->cols()!=inNodes) return false;
  sumW.mProd(inValues,&_W); //Producto matricial
  sumW.sumRow(&_b); //Suma de BIAS
  outValues.f1(actF,&sumW); //Aplicamos función de activación
  return true; //Todo Ok
}


//class RoJoNeuralNetworkQt------------------------------------------------------


void RoJoNeuralNetworkQt::end() {
  if(_layersCount>0) { //Si hay alguna capa...
    for(uint16_t i=0;i<_layersCount;i++) { //Recorremos todas las capas
      _layers[i]->end(); //Finalizamos la capa
      delete _layers[i]; //Borramos el objeto capa
    }
    delete[] _layers; //Borramos el array de capas
    _layersCount=0; //Anotamos que la red no tiene capas
  }
}

//Destructor
RoJoNeuralNetworkQt::~RoJoNeuralNetworkQt() {
  end();
}

//Init
//Parámetros
//- topology : Matriz de una sóla columna con número de nodos por capa
//- actF : Puntero a función de activación
//- costF : Puntero a función de coste
//Devuelve true si lo consigue
bool RoJoNeuralNetworkQt::begin(RoJoFloatMatrixQt *topology,float (*actF)(float),float (*costF)(float,float)) {
  if(topology->rows()<2 || topology->cols()!=1) return false;
  end();
  _layersCount=topology->rows()-1; //Anotamos número de capas
  _layers=new RoJoNeuralLayerQt*[_layersCount];
  for(uint16_t i=0;i<_layersCount;i++) { //Primero creamos las capas
    _layers[i]=new RoJoNeuralLayerQt;
  }
  for(uint16_t i=0;i<_layersCount;i++) { //Después las dimensionamos
    //Si tenemos algún error en el dimensionamiento...
    if(!_layers[i]->begin(topology->m[i][0],topology->m[i+1][0])) {
      end(); //No fallará porque ya están creadas todas las capas
      return false;
    }
  }
  for(uint16_t i=1;i<_layersCount;i++) { //Recorremos las capas a partir de la segunda
    //La matriz de entrada es la misma que la de salida de la capa anterior
    _layers[i]->inValues=&_layers[i-1]->outValues;
  }
  _actF=actF;
  _actFD=nullptr;
  _costF=costF;
  _costFD=nullptr;
  return true; //Todo Ok
}

//Cálculo de forwardPass en todas las capas
//Parámetros:
//- inValues: matriz de valores de entrada (samples,nodos de entrada de la primera capa)
//- outValues: matriz de salida (samples,nodos de salida de la última capa)
bool RoJoNeuralNetworkQt::calc(RoJoFloatMatrixQt *inValues,RoJoFloatMatrixQt *outValues) {
  if(_layersCount==0 || inValues->rows()==0 || inValues->cols()!=_layers[0]->inNodes) return false;
  _layers[0]->inValues=inValues; //Asignamos valores de entrada de la primera capa
  _layers[0]->calc(_actF); //Calculamos primera capa
  _layers[0]->sumW.end(); //Vaciamos matriz de suma ponderada para ahorrar memoria
  for(uint16_t layer=1;layer<_layersCount;layer++) { //Recorremos las capas a partir de la segunda
    _layers[layer]->calc(_actF); //Calculamos la capa actual
    //Para ahorrar memoria, eliminamos algunas matrices...
    _layers[layer]->inValues->end(); //Vaciamos valores de entrada
    _layers[layer]->sumW.end(); //Vaciamos suma ponderada
  }
  //Devolvemos una copia de los valores de salida de la última capa
  outValues->mCopy(&(_layers[_layersCount-1]->outValues));

  //Ya no necesitamos los valores de la salida de la última capa
  _layers[_layersCount-1]->outValues.end();
  return true; //Todo Ok
}

//Init train
//Parámetros:
//- actFD: Derivada de la función de activación
//- costFD: Derivada de la función de coste
//- lr: learning rate (ratio de aprendizaje)
//Devuelve true si lo consigue
bool RoJoNeuralNetworkQt::beginTrain(float (*actFD)(float),float (*costFD)(float,float),float lr) {
  if(_layersCount==0) return false;
  //Guardamos parámetros
  _actFD=actFD;
  _costFD=costFD;
  _lr=lr; //Guardamos learning rate
  return true;
}

//Entrenamiento de la red neuronal
//Parámetros:
//- inValues: matriz de valores de entrada (samples,nodos de entrada de la primera capa)
//- rightValues: matriz de valores de salida correctos (samples, nodos de salida de la última capa)
//- outValues: matriz de salida de última capa (samples,nodos de salida de la última capa)
//Devuelve true si lo consigue
bool RoJoNeuralNetworkQt::train(RoJoFloatMatrixQt *inValues,RoJoFloatMatrixQt *rightValues,RoJoFloatMatrixQt *outValues) {
  //Los parámetros no serán válidos si se cumple alguna de las siguientes condiciones:
  //- La red no está inicializada
  //- No hay valores de entrada
  //- Los valores de entrada no tienen tantos campos como nodos de entrada tiene la primera capa
  //- El número de valores de entrada es distinto de los de salida
  //- Los campos de los valores de salida no coinciden con el número de nodos de salida de la última capa
  if(_layersCount==0 || inValues->rows()==0 || inValues->cols()!=_layers[0]->inNodes || rightValues->rows()!=inValues->rows() || rightValues->cols()!=_layers[_layersCount-1]->outNodes) return false;
  //forwardPass conservando todos los resultados intermedios
  _layers[0]->inValues=inValues; //Asignamos valores de entrada de la primera capa
  for(uint16_t layer=0;layer<_layersCount;layer++) { //Recorremos todas las capas...
    _layers[layer]->calc(_actF); //...y las calculamos
  }
  //Devolvemos una copia de los valores de salida de la última capa
  outValues->mCopy(&(_layers[_layersCount-1]->outValues));
  //backwardPass. Recorreremos las capas al revés
  //Creamos matriz para guardar el resultado de la derivada de la función de
  //coste
  RoJoFloatMatrixQt delta;
  //Calculamos delta para la última capa
  delta.f2(_costFD,&(_layers[_layersCount-1]->outValues),rightValues);
  //Creamos matriz para guardar los coeficientes W de la capa previa
  //Puesto que antes de empezar, no tenemos valores coeficientes y después
  //se utilizan para multiplicar, creamos una matriz que no afecte a la
  //multiplicación
  uint16_t outNodesLastLayer=_layers[_layersCount-1]->outNodes;
  RoJoFloatMatrixQt Wprev(outNodesLastLayer,1);
  for(uint16_t i=0;i<outNodesLastLayer;i++) Wprev.m[i][0]=1;
  //Creamos matrices temporales para el cálculo de delta
  RoJoFloatMatrixQt tmp1,tmp2;
  //Recorremos todas las capas en orden inverso...
  for(int16_t layer=_layersCount-1;layer>=0;layer--) {
    tmp2.T(&Wprev); //Traspuesta
    tmp1.mProd(&delta,&tmp2); //Producto Matricial
    //Aplicamos la derivada de la función de activación sobre elresultado
    tmp2.f1(_actFD,&(_layers[layer]->outValues));
    delta.eProd(&tmp1,&tmp2); //Producto escalar
    Wprev.mCopy(&(_layers[layer]->_W)); //Guardamos W para utilizarlo en el siguiente ciclo
    tmp1.meanCols(&delta); //Calculamos promedio de cada columna de delta
    tmp1.eProd(-_lr); //Multiplicamos por el negativo de learning rate
    _layers[layer]->_b.mSum(&tmp1); //Lo añadimos al actual BIAS
    tmp1.T(_layers[layer]->inValues); //Traspuesta de valores de entrada
    tmp2.mProd(&tmp1,&delta); //Producto matricial con delta
    tmp2.eProd(-_lr); //Multiplicamos por el negativo de learning rate
    _layers[layer]->_W.mSum(&tmp2); //Lo añadimos al actual W
  }
  return true; //Todo Ok
}

//Calcula el error
//Parámetros:
//- outValues: matriz de salida de última capa (samples,nodos de salida de la última capa)
//- rightValues: matriz de valores de salida correctos (samples, nodos de salida de la última capa)
//- meanCost: matriz de error (promedio de resultados de la función de coste).
//Devuelve true si lo consigue
bool RoJoNeuralNetworkQt::cost(RoJoFloatMatrixQt *outValues,RoJoFloatMatrixQt *rightValues,RoJoFloatMatrixQt *meanCost) {
  //Las matrices de salida y valores correctos deben tener la misma dimensión
  if(outValues->cols()!=rightValues->cols() || outValues->rows()!=rightValues->rows()) return false;
  RoJoFloatMatrixQt costValues; //Guardará los resultados de la función de coste
  costValues.f2(_costF,outValues,rightValues); //Obtenemos los resultados de la función de coste
  meanCost->meanCols(&costValues); //Devolvemos el promedio de error por parámetro de salida
  return true; //Todo Ok
}

//Calculamos los valores de una colección de puntos definidos por una lista de
//coordenadas horizontales y verticales. Especialmente hecho para mapas de background
//Parámetros:
//- cXb : Lista de coordenadas horizontales
//- cYb : Lista de coordenadas verticales
//- Vb: Matriz en la que devolvemos los valores de los puntos
//Nota:
//- La dimensión de la matriz Vb viene dada por el número de elementos de las
//  listas de coordenadas. Filas=elementos de cYb. Columnas=elementos de cXb.
bool RoJoNeuralNetworkQt::mesh(RoJoFloatMatrixQt *cXb,RoJoFloatMatrixQt *cYb,RoJoFloatMatrixQt *Vb) {
  uint16_t rows=cYb->rows();
  uint16_t cols=cXb->rows();
  if(rows==0 || cols==0 || cYb->cols()!=1 || cXb->cols()!=1) return false;
  Vb->redim(rows,cols); //Dimensionamos la matriz de salida
  RoJoFloatMatrixQt X,Z; //Creamos matriz de entrada y salida para red neuronal
  float y; //Coordenada de fila
  X.redim(cols,2); //Introduciremos los datos por filas
  //Por lo tanto, la coordenada de columna permanecerá siempre constante
  //con independencia de la fila
  for(uint16_t col=0;col<cols;col++) X.m[col][0]=cXb->m[col][0];
  for(uint16_t row=0;row<rows;row++) { //Recorremos todas las filas...
    y=cYb->m[row][0]; //Calculamos la coordenada de fila
    //Será constante a todas las columnas
    for(uint16_t col=0;col<cols;col++) X.m[col][1]=y;
    calc(&X,&Z); //Calculamos valor
    //Copiamos el resultado a la matriz de valores
    for(uint16_t col=0;col<cols;col++) Vb->m[row][col]=Z.m[col][0];
  }
  return true; //Todo Ok
}
