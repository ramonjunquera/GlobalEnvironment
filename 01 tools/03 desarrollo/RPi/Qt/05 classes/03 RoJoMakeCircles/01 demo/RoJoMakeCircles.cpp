/*
 * Autor: Ramón Junquera
 * Decha: 20200611
 * Tema: Generador de puntos de anillos concéntricos
 */

#include "RoJoMakeCircles.h"

//Constructor
RoJoMakeCircles::RoJoMakeCircles() {
  srand(time(0)); //Inicializamos la semilla con el reloj
}

//Generador de números aleatorios entre [-maxValue,maxValue]
//Parámetro:
//- maxValue : valor límite
float RoJoMakeCircles::_rand(float maxValue) {
  return -maxValue+static_cast <float> (rand())/(static_cast <float> (RAND_MAX/2/maxValue));
}

//Genera puntos para un anillo
//Parámetros:
//- samples: Número de puntos del anillo
//- X,Y: Devuelve matrices de una columna y tantas filas como puntos tenga el
//  anillo, con las coordenadas x & y
//- radius: radio (por defecto 1)
//- noise: distancia de descentrado aleatorio máximo (por defecto 0)
//Devuelve true si lo consigue
bool RoJoMakeCircles::get(uint32_t samples,RoJoFloatMatrix *X,RoJoFloatMatrix *Y,float radius,float noise) {
  if(samples==0) return false;
  X->redim(samples,1);
  Y->redim(samples,1);
  float angleConversion=2*M_PI/samples;
  for(uint32_t p=0;p<samples;p++) {
    float angle=p*angleConversion;
    X->m[p][0]=radius*cos(angle)+_rand(noise);
    Y->m[p][0]=radius*sin(angle)+_rand(noise);
  }
  return true; //Todo Ok
}
