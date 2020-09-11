/*
 * Autor: Ramón Junquera
 * Decha: 20200610
 * Tema: Simulador de plt. Representación gráfica
 */

#include "RoJoQtPlt.h"

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

//Inicialización con dimensiones
//Devuelve true si lo consigue
bool RoJoQtPlt::begin(uint16_t xMax,uint16_t yMax) {
  if(xMax==0 || yMax==0) return false;
  end(); //Eliminamos cualquier gráfico anterior
  _xMax=xMax;
  _yMax=yMax;
  _pixmap=QPixmap(xMax,yMax);
  _painter=new QPainter(&_pixmap); //Creamos QPainter para gestionarlo
  _painter->fillRect(0,0,xMax,yMax,QColor(255,255,255)); //Fondo blanco
  //Fijamos valores de escalas para representar de [-1,1]
  _xMin=_yMin=-1;
  _xK=_xMax/2;
  _yK=_yMax/2;
  return true; //Todo Ok
}

//Muestra el gráfico en QLabel
void RoJoQtPlt::show() {
  _label->setPixmap(_pixmap);
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
//- X & Y : RoJoFloatMatrix de una sóla columna. Deben tener las mismas filas
//- c : color. Por defecto azul
//Respuesta: true si todo es correcto
bool RoJoQtPlt::scatter(RoJoFloatMatrix *X,RoJoFloatMatrix *Y,QColor c) {
  uint32_t rows=X->rows();
  if(X->cols()!=1 || Y->cols()!=1 || rows!=Y->rows()) return false;
  _painter->setPen(c); //Borde
  _painter->setBrush(c); //Relleno
  float x,y;
  //Dibujamos todos los puntos
  for(uint32_t row=0;row<rows;row++) {
    x=X->m[row][0];
    y=Y->m[row][0];
    _displayXY(&x,&y);
    _painter->drawEllipse(x-3,y-3,6,6);
  }
  return true; //Todo Ok
}

//Fija escalas y dibuja ejes
//Devuelve true si lo consigue
bool RoJoQtPlt::axis(float xMin,float xMax,float yMin,float yMax) {
  if(xMin>=xMax || yMin>=yMax) return false;
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
//Devuelve true si lo consigue
bool RoJoQtPlt::axis(bool scaled,RoJoFloatMatrix *X,RoJoFloatMatrix *Y) {
  if(X->cols()!=1 || Y->cols()!=1 || X->rows()!=Y->rows()) return false;
  float xMax,yMax;
  //Detectamos valores máximos y mínimos de cada eje
  X->maxmin(&_xMin,&xMax);
  Y->maxmin(&_yMin,&yMax);
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

void RoJoQtPlt::fill() {
  //Asignamos color negro al lápiz por defecto
  _painter->setPen(QColor(0,0,0)); //Negro
  //Dibujamos una rejilla
  for(int i=0;i<_xMax;i+=10) {
    _painter->drawLine(0,i,_xMax,i);
    _painter->drawLine(i,0,i,_yMax);
  }
}
