/*
 * Autor: Ramón Junquera
 * Fecha: 20200824
 * Tema: Generador de puntos de anillos concéntricos
 */

#include "RoJoMakeCirclesQt.h"

//Constructor
RoJoMakeCirclesQt::RoJoMakeCirclesQt() {
  srand(time(0)); //Inicializamos la semilla con el reloj
}

//Generador de números aleatorios entre [-maxValue,maxValue]
//Parámetro:
//- maxValue : valor límite
float RoJoMakeCirclesQt::_rand(float maxValue) {
  return -maxValue+static_cast <float> (rand())/(static_cast <float> (RAND_MAX/2/maxValue));
}

//Genera puntos para un anillo
//Parámetros:
//- res: puntero a matriz de respuesta
//- samples: Número de puntos del anillo
//- radius: radio (por defecto 1)
//- noise: distancia de descentrado aleatorio máximo (por defecto 0)
//Devuelve true si lo consigue
bool RoJoMakeCirclesQt::get(RoJoFloatMatrixQt *res,uint16_t samples,float radius,float noise) {
  if(samples==0) return false;
  res->redim(samples,2);
  float angleConversion=2*M_PI/samples;
  for(uint16_t p=0;p<samples;p++) {
    float angle=p*angleConversion;
    res->m[p][0]=radius*cos(angle)+_rand(noise);
    res->m[p][1]=radius*sin(angle)+_rand(noise);
  }
  return true;
}
