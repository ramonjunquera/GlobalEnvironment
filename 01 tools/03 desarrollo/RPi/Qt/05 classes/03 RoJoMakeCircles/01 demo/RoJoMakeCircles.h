/*
 * Autor: Ramón Junquera
 * Decha: 20200611
 * Tema: Generador de puntos de anillos concéntricos
 */

#ifndef RoJoMakeCircles_h
#define RoJoMakeCircles_h

#include <math.h> //Para M_PI
#include <cstdlib> //Para rand()
#include <time.h> //Para time()
#include <RoJoFloatMatrix.h> //Gestión de matrices

//Genera puntos de anillos centrados en el eje de coordenadas
class RoJoMakeCircles {
  private:
    float _rand(float maxValue); //Generador de números aleatorios
  public:
    RoJoMakeCircles(); //Constructor
    bool get(uint32_t samples,RoJoFloatMatrix *X,RoJoFloatMatrix *Y,float radius=1,float noise=0);
};

#endif
