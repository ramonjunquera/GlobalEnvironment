/*
 * Autor: Ramón Junquera
 * Decha: 20200610
 * Tema: Simulador de plt. Representación gráfica
 */

#ifndef RoJoQtPlt_h
#define RoJoQtPlt_h

#include <QLabel>
#include <QPainter>
#include <RoJoFloatMatrix.h>

class RoJoQtPlt {
  private:
    QLabel *_label; //Puntero a objeto QLabel que tiene el gráfico
    QPixmap _pixmap; //Gráfico
    QPainter *_painter; //Puntero a QPainter Para gestionar el gráfico
    uint16_t _xMax=0,_yMax=0; //Dimensiones iniciales
    float _xMin,_yMin,_xK,_yK; //Para cálculo de coordenadas
    void _displayXY(float *x,float *y); //Calcula coordenadas de pantalla
  public:
    RoJoQtPlt(QLabel *label); //Constructor
    ~RoJoQtPlt(); //Destructor
    void end();
    bool begin(uint16_t xMax=400,uint16_t yMax=400); //Inicialización con dimensiones
    void show();
    bool axis(float xMin=-1,float xMax=1,float yMin=-1,float yMax=1);
    bool axis(bool scaled,RoJoFloatMatrix *X,RoJoFloatMatrix *Y);
    bool scatter(RoJoFloatMatrix *X,RoJoFloatMatrix *Y,QColor c=QColor(0,0,255));
    void fill();
};

#endif
