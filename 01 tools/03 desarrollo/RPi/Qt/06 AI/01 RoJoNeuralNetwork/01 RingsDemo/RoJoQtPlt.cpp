/*
 * Autor: Ramón Junquera
 * Fecha: 20200824
 * Tema: Simulador de plt. Representación gráfica
 */

#include "RoJoQtPlt.h"

//Función que determina el color de un pixel del mapa de background
//La secuencia de colores es Red->White->Blue
//Esta función no pertenece a la clase para que el usuario pueda definir
//las suyas propias.
//Parámetros:
//- valueMin, valueMax : valores mínimo y máximo del rango
//- value: valor a analizar
//- c: color correspondiente al valor con la escala de colores
//Nota:
//Si el valor está fuera de rango, devolverá el color negro
void meshRWB(float valueMin,float valueMax,float value,QColor *c) {
  if(value<valueMin || value>valueMax) *c=QColor(0,0,0);
  else {
    float valueMean=(valueMax+valueMin)/2;
    float delta=valueMean-valueMin;
    if(value<valueMean) { //Tramo RW
      uint8_t level=(value-valueMin)/delta*255;
      *c=QColor(255,level,level);
    } else { //Tramo WB
      uint8_t level=(valueMax-value)/delta*255;
      *c=QColor(level,level,255);
    }
  }
}

//class RoJoQtPlt--------------------------------------------------------------

//Constructor
RoJoQtPlt::RoJoQtPlt(QLabel *label) {
  _label=label;
}

//Libera memoria
void RoJoQtPlt::end() {
  if(_xMax>0) { //Si hay gráfico...
    delete _painter;
    _xMax=_yMax=0; //El gráfico no tiene dimensiones
  }
}

//Destructor
RoJoQtPlt::~RoJoQtPlt() {
  end();
}

//Borra imágen
//Devuelve true si lo consigue
bool RoJoQtPlt::clear(QColor c) {
  if(_xMax==0) return false;
  _painter->fillRect(0,0,_xMax,_yMax,c);
  return true; //Todo Ok
}

//Inicialización con dimensiones en pixels
//Devuelve true si lo consigue
bool RoJoQtPlt::begin(uint16_t xMax,uint16_t yMax) {
  if(xMax==0 || yMax==0) return false;
  end(); //Eliminamos cualquier gráfico anterior
  _xMax=xMax;
  _yMax=yMax;
  _pixmap=QPixmap(xMax,yMax);
  _painter=new QPainter(&_pixmap); //Creamos QPainter para gestionarlo
  clear(); //Borramos imágen
  //Fijamos valores de escalas para representar de [-1,1]
  _xMin=_yMin=-1;
  _xK=_xMax/2;
  _yK=_yMax/2;
  //Damos valores por defecto al min y max de mesh
  //Utilizamos [0,1] porque son los límites de la función sigmoide
  meshMin=0;
  meshMax=1;
  return true; //Todo Ok
}

//Muestra el gráfico en QLabel
void RoJoQtPlt::show() {
  _label->setPixmap(_pixmap);
  _label->repaint();

}

//Calcula coordenadas de pantalla
//Parámetros
//- input: coordenada espaciales
//- output: coordenadas de pantalla
void RoJoQtPlt::_displayXY(float *x,float *y) {
  *x=(*x-_xMin)*_xK;
  *y=_yMax-(*y-_yMin)*_yK;
}

//Representación gráfica de puntos
//Parámetros:
//- X : RoJoFloatMatrixQt con coordenadas de tamaño (samples,2)
//- c : color. Por defecto azul
//Respuesta: true si todo es correcto
bool RoJoQtPlt::scatter(RoJoFloatMatrixQt *X,QColor c) {
  if(X->cols()!=2) return false;
  //_painter->setPen(c); //Borde
  _painter->setPen(QColor(0,0,0)); //Borde
  _painter->setBrush(c); //Relleno
  uint16_t rows=X->rows();
  float x,y;
  //Dibujamos todos los puntos
  for(uint16_t row=0;row<rows;row++) {
    x=X->m[row][0];
    y=X->m[row][1];
    _displayXY(&x,&y);
    _painter->drawEllipse(x-3,y-3,6,6);
  }
  return true; //Todo Ok
}

//Representación gráfica de líneas
//Parámetros:
//- X : RoJoFloatMatrixQt con coordenadas de tamaño (samples,2)
//- c : color. Por defecto azul
//Respuesta: true si todo es correcto
bool RoJoQtPlt::plot(RoJoFloatMatrixQt *X,QColor c) {
  uint16_t rows=X->rows();
  if(X->cols()!=2) return false;
  _painter->setPen(QColor(0,0,0)); //Borde
  _painter->setBrush(c); //Relleno
  float x0,y0,x1,y1;
  //Primer punto
  x1=X->m[0][0];
  y1=X->m[0][1];
  _displayXY(&x1,&y1);
  //Dibujamos las líneas que unen los puntos
  for(uint16_t row=1;row<rows;row++) {
    x0=x1;
    y0=y1;
    x1=X->m[row][0];
    y1=X->m[row][1];
    _displayXY(&x1,&y1);
    _painter->drawLine(x0,y0,x1,y1);
  }
  return true; //Todo Ok
}

//Fija escalas y dibuja ejes
//Devuelve true si lo consigue
bool RoJoQtPlt::axis(float xMin,float xMax,float yMin,float yMax) {
  if(xMin>=xMax || yMin>=yMax) return false;
  clear(); //Borramos imágen
  _xMin=xMin;
  _yMin=yMin;
  _xK=(float)_xMax/(xMax-xMin);
  _yK=(float)_yMax/(yMax-yMin);
  //Dibujamos los ejes (si son visibles)
  float x,y;
  x=y=0; //Centro de coordenadas
  _displayXY(&x,&y);
  _painter->setPen(QColor(0,0,0)); //Negro
  if(x>=0 && x<_xMax) _painter->drawLine(x,0,x,_yMax);
  if(y>=0 && y<_yMax) _painter->drawLine(0,y,_xMax,y);
  return true; //Todo Ok
}

//Fija escalas y dibuja ejes en base a coordenadas a mostrar
//Parámetros:
//- scaled: false=ejes con la misma proporción
//- X: matrix con tamaño (samples,2)
//Devuelve true si lo consigue
bool RoJoQtPlt::axis(bool scaled,RoJoFloatMatrixQt *X) {
  if(X->cols()!=2) return false;
  clear(); //Borramos imágen
  float xMax,yMax;
  //Detectamos valores máximos y mínimos de cada eje
  X->maxminCol(&_xMin,&xMax,0); //maxmin de x
  X->maxminCol(&_yMin,&yMax,1); //maxmin de y
  //Si la amplitud del intervalo es nulo en algún caso...
  //...marcamos nosotros los límites
  if(_xMin==xMax) {
    _xMin-=1;
    xMax+=1;
  }
  if(_yMin==yMax) {
    _yMin-=1;
    yMax+=1;
  }
  //Calculamos amplitud de intervalo y tomamos el 5% para dar márgen
  float xDelta=0.05*(xMax-_xMin);
  float yDelta=0.05*(yMax-_yMin);
  //Recalculamos los límites del intervalo
  _xMin-=xDelta;
  xMax+=xDelta;
  _yMin-=yDelta;
  yMax+=yDelta;
  if(scaled) { //Si los ejes pueden tener distinta escala...
    //Calculamos la K para el cálculo de coordenadas de pantalla
    _xK=(float)_xMax/(xMax-_xMin);
    _yK=(float)_yMax/(yMax-_yMin);
  } else { //Los ejes no pueden tener distinta escala
    float delta=(xMax-_xMin-yMax+_yMin)/2;
    if(xDelta>yDelta) {
      _yMin-=delta;
      yMax+=delta;
    } else {
      _xMin+=delta;
      xMax-=delta;
    }
    delta=xMax-_xMin;
    _xK=(float)_xMax/delta;
    _yK=(float)_yMax/delta;
  }
  //Dibujamos los ejes (si son visibles)
  float x,y;
  x=y=0; //Centro de coordenadas
  _displayXY(&x,&y);
  _painter->setPen(QColor(0,0,0)); //Negro
  if(x>=0 && x<_xMax) _painter->drawLine(x,0,x,_yMax);
  if(y>=0 && y<_yMax) _painter->drawLine(0,y,_xMax,y);
  return true; //Todo Ok
}

//Obtiene los límites de visualización del eje X
void RoJoQtPlt::getLimitsX(float *xMin,float *xMax) {
  *xMin=_xMin;
  *xMax=_xMin+_xMax/_xK;
}

//Obtiene los límites de visualización del eje Y
void RoJoQtPlt::getLimitsY(float *yMin,float *yMax) {
  *yMin=_yMin;
  *yMax=_yMin+_yMax/_yK;
}

//Representación de mapa de color
//Parámetros:
//- X: Matriz de coordenadas X (cols,1)
//- Y: Matriz de coordenadas Y (rows,1)
//- V: Matriz de valores (rows,cols);
//- void (*meshColor)(float,float,float,QColor*): función de cálculo de color
//- autoLimit: cálculo automático de límites?
//Nota:
//- Suponemos que las coordenadas están ordenadas de menor a mayor
bool RoJoQtPlt::mesh(RoJoFloatMatrixQt *X,RoJoFloatMatrixQt *Y,RoJoFloatMatrixQt *V,void (*meshColor)(float,float,float,QColor*),bool autoLimit) {
  uint16_t rows=Y->rows();
  uint16_t cols=X->rows();
  if(rows==0 || cols==0 || X->cols()!=1 || Y->cols()!=1 || V->rows()!=rows || V->cols()!=cols) return false;
  if(autoLimit) V->maxmin(&meshMin,&meshMax); //Si hay que calcular los límites...lo hacemos
  //Definimos arrays de enteros para guardar la conversión a coordenadas de pantalla
  //Esto evitará que tengamos que convertirlas cada vez que queramos usarlas
  //El primer elemento es el límite inferior visible
  //El último elemento es el límite superior visible
  //Los elementos intermedios contienen el promedio de dos valores contiguos
  //El resultado final serán las coordenadas de pantalla de las separaciones de pixels
  uint16_t *Xp=new uint16_t[cols+1];
  uint16_t *Yp=new uint16_t[rows+1];
  Xp[0]=0; Xp[cols]=_xMax;
  Yp[0]=_yMax; Yp[rows]=0; //El eje y está invertido
  float meanValue;
  QColor c; //Color del pixel
  for(uint16_t i=0;i<cols-1;i++) {
    meanValue=(X->m[i][0]+X->m[i+1][0])/2;
    Xp[i+1]=(meanValue-_xMin)*_xK;
  }
  for(uint16_t i=0;i<rows-1;i++) {
    meanValue=(Y->m[i][0]+Y->m[i+1][0])/2;
    Yp[i+1]=_yMax-(meanValue-_yMin)*_yK;
  }
  uint16_t x,y; //Coordenadas de pantalla temporales
  for(uint32_t row=0;row<rows;row++) {
    for(uint32_t col=0;col<cols;col++) {
      meshColor(meshMin,meshMax,V->m[row][col],&c); //Calculamos color de pixel
      _painter->setBrush(c); //Relleno
      _painter->setPen(c); //Borde
      //Tenemos las coordenadas de pantalla de los límites de los pixels
      //en Xp & Yp.
      //Las coordenadas de Yp están invertidas (de mayor a menor) para que
      //se muestra en pantalla con el sentido de coordenadas cartesianas.
      //La función drawRect tiene como parámetros el vértice inicial,
      //la anchura y la altura.
      //Intentamos tomar como vértice inicial el superior izquierdo
      x=Xp[col]; y=Yp[row+1];
      _painter->drawRect(x,y,Xp[col+1]-x,Yp[row]-y);
    }
  }
  //Ya no necesitamos los arrays de coordenadas de pantalla
  delete[] Xp;
  delete[] Yp;
  return true; //Todo Ok
}
