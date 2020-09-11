/*
 * Autor: Ramón Junquera
 * Decha: 20200611
 * Tema: Gestión de matrices bidimensionales de tipo float
 */

#include "RoJoFloatMatrix.h"

//Descarga memoria
void RoJoFloatMatrix::end() {
  if(_rows>0) { //Si tenemos algo guardado en la matriz...
    //Recorremos todas las filas...y las borramos
    for(uint32_t i=0;i<_rows;i++) delete[] m[i];
    delete[] m; //Borramos la propia matriz
    _rows=_cols=0; //Anotamos que la matriz está vacía
  }
}

//Destructor
RoJoFloatMatrix::~RoJoFloatMatrix() {
  end();
}

//Redimensionar
//Parámetros:
//- rows : número de filas
//- cols : número de columnas
//Respuesta: true si lo consigue
bool RoJoFloatMatrix::redim(uint32_t rows,uint32_t cols) {
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
  for(uint32_t i=0;i<rows;i++) m[i]=new float[cols];
  return true; //Todo Ok
}

//Constructor con tamaño
RoJoFloatMatrix::RoJoFloatMatrix(uint32_t rows,uint32_t cols) {
  redim(rows,cols); //Asignamos tamaño
}

//Constructor para asignación
RoJoFloatMatrix::RoJoFloatMatrix(const RoJoFloatMatrix &A) {
  //Dimensionamos la matrix actual como la del parámetro
  redim(A._rows,A._cols);
  //Copiamos todos sus valores
  for(uint32_t row=0;row<_rows;row++) {
    for(uint32_t col=0;col<_cols;col++) m[row][col]=A.m[row][col];
  }
}

//Devuelve el número de columnas
uint32_t RoJoFloatMatrix::cols() {
  return _cols;
}

//Devuelve el número de filas
uint32_t RoJoFloatMatrix::rows() {
  return _rows;
}

//Genera la matriz traspuesta
RoJoFloatMatrix& RoJoFloatMatrix::T() {
  RoJoFloatMatrix *res=new RoJoFloatMatrix(_cols,_rows);
  for(uint32_t row=0;row<_rows;row++) {
    for(uint32_t col=0;col<_cols;col++) {
      res->m[col][row]=m[row][col];
    }
  }
  return *res;
}

//Llena la matriz con valores aleatorios entre dos límites
//Respuesta: true si lo consigue
bool RoJoFloatMatrix::fill(float minValue,float maxValue) {
  //Si los límites no son correctos...error
  if(minValue>maxValue) return false;
  //Los límites son correctos
  srand(time(0)); //Inicializamos la semilla con el reloj
  for(uint32_t row=0;row<_rows;row++) {
    for(uint32_t col=0;col<_cols;col++) {
      m[row][col]=minValue+static_cast <float> (rand())/(static_cast <float> (RAND_MAX/(maxValue-minValue)));
    }
  }
  return true; //Todo Ok
}

//Calcula máx y min de una matrix
//Devuelve true si lo consigue
bool RoJoFloatMatrix::maxmin(float *min,float *max) {
  if(_rows==0) return false;
  *min=*max=m[0][0]; //Inicializamos max & min
  for(uint32_t row=0;row<_rows;row++) {
    for(uint32_t col=0;col<_cols;col++) {
      if(m[row][col]<*min) *min=m[row][col];
      if(m[row][col]>*max) *max=m[row][col];
    }
  }
  return true; //Todo Ok
}

//Definición de operador + con otra matriz
RoJoFloatMatrix& RoJoFloatMatrix::operator + (RoJoFloatMatrix &A) {
  //A la matriz actual se le quiere sumar otra matriz
  //Creamos objeto de respuesta vacío
  RoJoFloatMatrix *res=new RoJoFloatMatrix;
  //Si tienen el mismo tamaño...
  if(_rows==A._rows && _cols==A._cols) {
    res->redim(_rows,_cols); //Asignamos tamaño
    //Sumamos cada elemento
    for(uint32_t row=0;row<_rows;row++) {
      for(uint32_t col=0;col<_cols;col++) {
        res->m[row][col]=m[row][col]+A.m[row][col];
      }
    }
  }
  return *res; //Devolvemos el objeto creado como respuesta
}

//Definición de operador * con valor numérico
RoJoFloatMatrix& RoJoFloatMatrix::operator * (float v) {
  //A la matriz actual se le multiplica por un escalar
  //Creamos objeto de respuesta vacío y del mismo tamaño
  RoJoFloatMatrix *res=new RoJoFloatMatrix(_rows,_cols);
  //Multiplicamos cada elemento
  for(uint32_t row=0;row<_rows;row++) {
    for(uint32_t col=0;col<_cols;col++) {
      res->m[row][col]=m[row][col]*v;
    }
  }
  return *res; //Devolvemos el objeto creado como respuesta
}

//Definición de operador * con otra matriz (producto matricial)
RoJoFloatMatrix& RoJoFloatMatrix::operator * (RoJoFloatMatrix &A) {
  RoJoFloatMatrix *res=new RoJoFloatMatrix;
  //Es imprescindible que el número de columnas de la primera sea
  //igual que el número de filas de las segunda
  if(_cols==A._rows) {
    res->redim(_rows,A._cols);
    for(uint32_t row=0;row<_rows;row++) {
      for(uint32_t col=0;col<A._cols;col++) {
        float sum=0;
        for(uint32_t i=0;i<_cols;i++) sum+=m[row][i]*A.m[i][col];
        res->m[row][col]=sum;
      }
    }
  }
  return *res; //Devolvemos el objeto creado como respuesta
}

//Aplica una función a todos los elementos de la matriz
//La función debe tener un sólo parámetros de tipo float
//y devolverá un float.
RoJoFloatMatrix& RoJoFloatMatrix::f(float (*f)(float)) {
  //Creamos la matriz de respuesta con el mismo tamaño
  RoJoFloatMatrix *res=new RoJoFloatMatrix(_rows,_cols);
  //Recorremos todos los elementos y aplicamos función
  for(uint32_t row=0;row<_rows;row++) {
    for(uint32_t col=0;col<_cols;col++) {
      res->m[row][col]=f(m[row][col]);
    }
  }
  return *res;
}

//Definición de operador = para asignación de valor matricial
RoJoFloatMatrix& RoJoFloatMatrix::operator = (RoJoFloatMatrix &A) {
  //Redimensionamos el objeto actual con el tamaño del parámetro
  redim(A._rows,A._cols);
  //Copiamos todos los datos de la matriz parámetro al actual
  for(uint32_t row=0;row<_rows;row++) {
    for(uint32_t col=0;col<_cols;col++) {
      m[row][col]=A.m[row][col];
    }
  }
  return *this; //Devolvemos el propio objeto
}
