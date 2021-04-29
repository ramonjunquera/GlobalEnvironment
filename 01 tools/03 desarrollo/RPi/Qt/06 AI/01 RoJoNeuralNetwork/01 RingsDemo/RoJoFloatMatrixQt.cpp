/*
 * Autor: Ramón Junquera
 * Fecha: 20200911
 * Tema: Gestión de matrices bidimensionales de tipo float
 */

#include "RoJoFloatMatrixQt.h"

//Descarga memoria
void RoJoFloatMatrixQt::end() {
  if(_rows>0) { //Si tenemos algo guardado en la matriz...
    //Recorremos todas las filas...y las borramos
    for(uint16_t i=0;i<_rows;i++) delete[] m[i];
    delete[] m; //Borramos la propia matriz
    _rows=_cols=0; //Anotamos que la matriz está vacía
  }
}

//Destructor
RoJoFloatMatrixQt::~RoJoFloatMatrixQt() {
  end();
}

//Redimensionar
//Parámetros:
//- rows : número de filas
//- cols : número de columnas
//Respuesta: true si lo consigue
bool RoJoFloatMatrixQt::redim(uint16_t rows,uint16_t cols) {
  //Si alguna de las dimensiones es nula...error
  if(rows==0 || cols==0) return false;
  //El valor de las dimensiones es correcto
  end(); //Nos aseguramos que la matriz está vacía
  //Anotamos las dimensiones en las variables privadas
  _rows=rows; _cols=cols;
  //Creamos un array de punteros con tantos elementos como columnas
  m=new float*[rows];
  //Creamos un array para posición de _m con tantos elementos como
  //columnas
  for(uint16_t i=0;i<rows;i++) m[i]=new float[cols];
  return true; //Todo Ok
}

//Constructor con tamaño
RoJoFloatMatrixQt::RoJoFloatMatrixQt(uint16_t rows,uint16_t cols) {
  redim(rows,cols); //Asignamos tamaño
}

//Constructor para asignación
RoJoFloatMatrixQt::RoJoFloatMatrixQt(RoJoFloatMatrixQt &A) {
  //Dimensionamos la matrix actual como la del parámetro
  redim(A._rows,A._cols);
  mCopy(&A);
}

//Llena la matrix con una secuencia en un rango
//El primer valor coincidirá con minValue
//Y el último con maxValue
bool RoJoFloatMatrixQt::linspace(float minValue,float maxValue,uint16_t samples) {
  if(maxValue<=minValue || samples<2) return false;
  //Dimensionamos matriz con tantas filas como muestras y una sola columna
  redim(samples,1);
  //Calculamos sus valores
  float k=(maxValue-minValue)/(samples-1);
  for(uint16_t i=0;i<samples;i++) m[i][0]=minValue+i*k;
  return true; //Todo Ok
}

//Devuelve el número de columnas
uint16_t RoJoFloatMatrixQt::cols() {
  return _cols;
}

//Devuelve el número de filas
uint16_t RoJoFloatMatrixQt::rows() {
  return _rows;
}

//Traspone matriz
bool RoJoFloatMatrixQt::T(RoJoFloatMatrixQt *A) {
  if(!redim(A->_cols,A->_rows)) return false;
  float **mA=A->m; //Puntero a array de A
  float *mr; //Puntero a fila de matriz actual
  for(uint16_t row=0;row<_rows;row++) {
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) mr[col]=mA[col][row];
  }
  return true; //Todo Ok
}

//Llena la matriz con valores aleatorios entre dos límites
//Respuesta: true si lo consigue
bool RoJoFloatMatrixQt::fillRand(float minValue,float maxValue) {
  if(minValue>=maxValue) return false; //Si los límites no son correctos...error
  srand(time(0)); //Inicializamos la semilla con el reloj
  float *mr; //Puntero a fila de matriz actual
  float delta=((float)RAND_MAX)/(maxValue-minValue);
  for(uint16_t row=0;row<_rows;row++) {
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) mr[col]=minValue+((float)rand()/delta);
  }
  return true; //Todo Ok
}

//Llena la matriz con un valor
//Respuesta: true si lo consigue
bool RoJoFloatMatrixQt::fill(float value) {
  float *mr; //Puntero a fila de matriz actual
  for(uint16_t row=0;row<_rows;row++) {
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) mr[col]=value;
  }
  return true; //Todo Ok
}

//Calcula máx y min de un rango de columnas
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::maxminRangeCol(float *min,float *max,uint16_t firstCol,uint16_t lastCol) {
  if(_rows==0 || firstCol>lastCol || lastCol>=_cols) return false;
  *min=*max=m[0][firstCol]; //Valores inicial
  float *mr; //Puntero a fila de matriz actual
  float value;
  for(uint16_t row=0;row<_rows;row++) {
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=firstCol;col<=lastCol;col++) {
      value=mr[col];
      if(value<*min) *min=value;
      if(value>*max) *max=value;
    }
  }
  return true; //Todo Ok
}

//Calcula máx y min de una matrix
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::maxmin(float *min,float *max) {
  return maxminRangeCol(min,max,0,_cols-1);
}

//Calcula máx y min de una columna
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::maxminCol(float *min,float *max,uint16_t col) {
  return maxminRangeCol(min,max,col,col);
}

//Producto escalar
//Multiplica los elementos de dos matrices de igual dimensión
bool RoJoFloatMatrixQt::eProd(RoJoFloatMatrixQt *A, RoJoFloatMatrixQt *B) {
  if(A->_rows==0 || A->_rows!=B->_rows || A->_cols!=B->_cols) return false;
  if(!redim(A->_rows,B->_cols)) return false;
  float *mrA,*mrB,*mr; //Punteros a filas
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mrA=A->m[row]; //Anotamos fila de matriz A
    mrB=B->m[row]; //Anotamos fila de matriz B
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) mr[col]=mrA[col]*mrB[col];
  }
  return true; //Todo Ok
}

//Producto escalar
//Multiplica todos los elementos de la matriz por un float
bool RoJoFloatMatrixQt::eProd(float v) {
  if(_rows==0) return false;
  float *mr; //Puntero a fila de matriz actual
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mr=m[row]; //Anotamos fila de matriz Z
    for(uint16_t col=0;col<_cols;col++) mr[col]*=v;
  }
  return true; //Todo Ok
}

//Suma matricial. Añade A a la matriz actual
bool RoJoFloatMatrixQt::mSum(RoJoFloatMatrixQt *A) {
  //Ambas matrices deben tener dimensión y coincidir
  if(_rows!=A->_rows || _cols!=A->_cols || _rows==0 || _cols==0) return false;
  float *mrA; //Puntero a fila de A
  float *mr; //Puntero a fila de matriz actual
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mrA=A->m[row]; //Anotamos fila de matriz A
    mr=m[row]; //Anotamos fila de matriz actual
    //Recorremos columnas y sumamos A a Z (Z=Z+A)
    for(uint16_t col=0;col<_cols;col++) mr[col]+=mrA[col];
  }
  return true; //Todo Ok
}

//Producto matricial A*B
bool RoJoFloatMatrixQt::mProd(RoJoFloatMatrixQt *A,RoJoFloatMatrixQt *B) {
  //Comprobamos que las dimensiones son correctas
  if(A->_cols!=B->_rows || A->_rows==0 || A->_cols==0 || B->_cols==0) return false;
  if(!redim(A->_rows,B->_cols)) return false;
  float *mrA; //Puntero a fila de A
  float *mr; //Puntero a fila de matriz actual
  uint16_t items=A->_cols; //Obtenemos el número de items (columnas de A)
  float sum;
  float **mB=B->m; //Puntero de array de B
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mrA=A->m[row]; //Anotamos fila de matriz A
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) {
      sum=0; //Comenzamos a sumar
      for(uint16_t i=0;i<items;i++) sum+=mrA[i]*mB[i][col];
      mr[col]=sum;
    }
  }
  return true; //Todo Ok
}

//Crea el resultado de aplicar una función a una matriz
//Parámetros:
//- f: puntero a función de un sólo parámetro float que devuelve un float
//- P: puntero a matriz usada como parámetro de la función
bool RoJoFloatMatrixQt::f1(float (*f)(float),RoJoFloatMatrixQt *P) {
  if(!redim(P->_rows,P->_cols)) return false;
  float *mrP,*mr; //Punteros a filas
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mrP=P->m[row]; //Anotamos fila de matriz A
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) mr[col]=f(mrP[col]);
  }
  return true; //Todo Ok
}

//Crea el resultado de aplicar una función a una matriz
//Parámetros:
//- f: puntero a función de dos parámetros float que devuelve un float
//- P0: puntero a matriz usada como primer parámetro de la función
//- P1: puntero a matriz usada como segundo parámetro de la función
bool RoJoFloatMatrixQt::f2(float (*f)(float,float),RoJoFloatMatrixQt *P0,RoJoFloatMatrixQt *P1) {
  if(P0->_cols!=P1->_cols || P0->_rows!=P1->_rows) return false;
  if(!redim(P0->_rows,P0->_cols)) return false;
  float *mrP0,*mrP1,*mr; //Punteros a filas
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mrP0=P0->m[row]; //Anotamos fila de matriz parámetro 0
    mrP1=P1->m[row]; //Anotamos fila de matriz parámetro 1
    mr=m[row]; //Anotamos fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) mr[col]=f(mrP0[col],mrP1[col]);
  }
  return true; //Todo Ok
}

//Copia matricial A en local con delta
void RoJoFloatMatrixQt::_mCopy(RoJoFloatMatrixQt *A,uint16_t deltaRows,uint16_t deltaCols) {
  float *mrA; //Puntero a fila de A
  float *mr; //Puntero a fila de matriz actual
  uint16_t rowsA=A->_rows;
  uint16_t colsA=A->_cols;
  for(uint16_t row=0;row<rowsA;row++) { //Recorremos todas las filas de A...
    mrA=A->m[row]; //Anotamos fila de matriz A
    mr=m[row+deltaRows]; //Anotamos fila de matriz Z
    for(uint16_t col=0;col<colsA;col++) mr[col+deltaCols]=mrA[col];
  }
}

//Copia matriz A en local
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::mCopy(RoJoFloatMatrixQt *A) {
  if(!redim(A->_rows,A->_cols)) return false;
  _mCopy(A,0,0); //Copiamos A sin deltas
  return true; //Todo Ok
}

//Pega filas de A y B guardando el resultado en la matriz actual
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::glueRows(RoJoFloatMatrixQt *A,RoJoFloatMatrixQt *B) {
  if(A->_cols!=B->_cols) return false;
  if(!redim(A->_rows+B->_rows,A->_cols)) return false;
  _mCopy(A,0,0); //Copiamos A sin deltas
  _mCopy(B,A->_rows,0); //Copiamos B desplazada tantas filas como tenga A
  return true; //Todo Ok
}

//Pega columnas de A y B guardando el resultado en la matriz actual
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::glueCols(RoJoFloatMatrixQt *A,RoJoFloatMatrixQt *B) {
  if(A->_rows!=B->_rows) return false;
  if(!redim(A->_rows,A->_cols+B->_cols)) return false;
  _mCopy(A,0,0); //Copiamos A sin deltas
  _mCopy(B,0,A->_cols); //Copiamos B desplazada tantas columnas como tenga A
  return true; //Todo Ok
}

//Suma la matriz columna a todas las columnas
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::sumCol(RoJoFloatMatrixQt *A) {
  if(_rows!=A->_rows || A->_cols!=1) return false;
  float v;
  float **mA=A->m; //Puntero de array de A
  float *mr; //Puntero a fila de matriz actual
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mr=m[row]; //Anotamos fila de matriz actual
    v=mA[row][0];
    for(uint16_t col=0;col<_cols;col++) mr[col]+=v;
  }
  return true; //Todo Ok
}

//Suma la matriz fila a todas las filas
//Devuelve true si lo consigue
bool RoJoFloatMatrixQt::sumRow(RoJoFloatMatrixQt *A) {
  if(_cols!=A->_cols || A->_rows!=1) return false;
  float *mrA=A->m[0]; //Puntero a primera fila de A
  float *mr; //Puntero a fila de matriz actual
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mr=m[row]; //Puntero a fila de matriz actual
    for(uint16_t col=0;col<_cols;col++) mr[col]+=mrA[col];
  }
  return true; //Todo Ok
}

//Crea secuencia desordenada de números enteros con tantos elementos como
//filas tenga la matriz
//La secuencia siempre comienza desde 0
//Devuelve el puntero del array resultante
uint16_t* RoJoFloatMatrixQt::scrambleList() {
  uint16_t *res=nullptr; //Respuesta
  if(_rows>0) {
    res=new uint16_t[_rows]; //Creamos array de respuesta
    uint16_t *tList=new uint16_t[_rows]; //Creamos array temporal
    for(uint16_t i=0;i<_rows;i++) tList[i]=i; //Lo llenamos con secuencia ordenada
    srand(time(0)); //Inicializamos la semilla con el reloj
    for(uint16_t i=0;i<_rows;i++) { //Recorremos todos los números
      uint16_t index=rand() % (_rows-i);
      res[i]=tList[index];
      for(uint16_t a=index;a<(_rows-i-1);a++) tList[a]=tList[a+1];
    }
    delete[] tList; //Liberamos array temporal
  }
  return res;
}

//Desordena la matriz en base a la lista
bool RoJoFloatMatrixQt::scramble(uint16_t *sList) {
  //Suponemos que la lista tiene los mismos elementos que la matriz
  RoJoFloatMatrixQt A; //Creamos matriz temporal
  A.mCopy(this); //Copiamos matriz actual a la temporal
  float *mr; //Puntero a fila de matriz actual
  float **mA=A.m; //Puntero a array de datos de A
  for(uint16_t row=0;row<_rows;row++) { //Recorremos todas las filas...
    mr=m[row];
    for(uint16_t col=0;col<_cols;col++) mr[col]=mA[sList[row]][col];
  }
  return true; //Todo Ok
}

//Valor promedio de las columnas
//El resultado es una matriz de una sola fila y tantas columnas como la del parámetro
bool RoJoFloatMatrixQt::meanCols(RoJoFloatMatrixQt *A) {
  if(A->_rows==0) return false;
  if(!redim(1,A->_cols)) return false;
  float sum;
  uint16_t rows=A->_rows;
  for(uint16_t col=0;col<_cols;col++) {
    sum=0;
    for(uint16_t row=0;row<rows;row++) sum+=A->m[row][col];
    m[0][col]=sum/rows;
  }
  return true; //Todo Ok
}
