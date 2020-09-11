/*
 * Autor: Ramón Junquera
 * Fecha: 20200824
 * Tema: Generador de puntos de anillos concéntricos
 */

#ifndef RoJoMakeCirclesQt_h
#define RoJoMakeCirclesQt_h

#include <stdint.h> //Para tipos standard
#include <math.h> //Para M_PI
#include <cstdlib> //Para rand()
#include <time.h> //Para time() inicializando semilla de números aleatorios
#include <RoJoFloatMatrixQt.h> //Gestión de matrices

//Genera puntos de anillos centrados en el eje de coordenadas
class RoJoMakeCirclesQt {
  private:
    float _rand(float maxValue); //Generador de números aleatorios
  public:
    RoJoMakeCirclesQt(); //Constructor
    bool get(RoJoFloatMatrixQt *res,uint16_t samples,float radius,float noise);
};

#endif
