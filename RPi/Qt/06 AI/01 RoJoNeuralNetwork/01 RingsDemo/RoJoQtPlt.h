/*
 * Autor: Ramón Junquera
 * Fecha: 20200824
 * Tema: Simulador de plt. Representación gráfica
 */

#ifndef RoJoQtPlt_h
#define RoJoQtPlt_h

#include <QLabel>
#include <QPainter>
#include <stdint.h> //Para tipos standard
#include <RoJoFloatMatrixQt.h> //Gestión de matrices

//Función que determina el color de un pixel del mapa de background
void meshRWB(float valueMin,float valueMax,float value,QColor *c);

class RoJoQtPlt {
  private:
    QLabel *_label; //Puntero a objeto QLabel que tiene el gráfico
    QPixmap _pixmap; //Gráfico
    QPainter *_painter; //Puntero a QPainter Para gestionar el gráfico
    uint16_t _xMax=0,_yMax=0; //Dimensiones iniciales en pixels
    float _xMin,_yMin,_xK,_yK; //Para cálculo de coordenadas
    void _displayXY(float *x,float *y); //Calcula coordenadas de pantalla

  public:
    RoJoQtPlt(QLabel *label); //Constructor
    ~RoJoQtPlt(); //Destructor
    void end();
    bool begin(uint16_t xMax=400,uint16_t yMax=400); //Inicialización con dimensiones
    bool clear(QColor c=QColor(255,255,255)); //Borra imágen
    void show();
    bool axis(float xMin=-1,float xMax=1,float yMin=-1,float yMax=1);
    bool axis(bool scaled,RoJoFloatMatrixQt *X);
    bool scatter(RoJoFloatMatrixQt *X,QColor c=QColor(0,0,255));
    bool plot(RoJoFloatMatrixQt *X,QColor c=QColor(0,0,255));
    void getLimitsX(float *xMin,float *xMax);
    void getLimitsY(float *yMin,float *yMax);
    float meshMin,meshMax;
    bool mesh(RoJoFloatMatrixQt *X,RoJoFloatMatrixQt *Y,RoJoFloatMatrixQt *V,void (*meshColor)(float,float,float,QColor*),bool autoLimit=true); //Muestra mapa de color
};

#endif
