/*
  Autor: Ramón Junquera
  Fecha: 20200219
  Tema: Acelerómetro 
  Objetivo: Mostrar lecturas gráficamente
  Material: M5Stack Atom Matrix

  Descripción:
    Mostraremos la imagen de una bola en el display y la posición del dispositivo se tendrá en cuenta en su
    movimiento.
    No ulizaremos el eje Z en este desarrollo.
    El sistema se calibra solo. Supone que la posición inicial es la de reposo.
    Se añade efecto rebote contra los límites del display para que sea más fácil
    controlar la bola y para que el comportamiento de la bola sea más realista.

  Resultado:
    Vemos que la bola del display responde a las inclinaciones del dispositivo
*/

#include <Arduino.h>
#include <RoJoNeoPixel.h>
#include <RoJoSprite.h>
#include <RoJoMPU9250.h>

//Definición de constantes globales
const float displaySize[]={5,5}; //Tamaño de display (restando tamaño de sprite)
const float elastic=0.8; //Coeficiente de elasticidad. 0=no rebota. 1=siempre rebota
const float accel=2000000; //Coeficiente de aceleración. Cuanto más pequeño, más rápido

//Definición de variables globales
RoJoNeoPixel display;
RoJoMPU9250 GAM; //Sensores
int16_t dataA[3]; //Datos del acelerómetro
int16_t refXY[2]; //Valores de referencia
float ballXY[2]; //Coordenadas del sprite
float ballSpeed[2]; //Velocidad de la bola

void setup() {
  //Inicialización de NeoPixel para Atom Matrix
  display.begin(5,5,27);
  //Inicialización de sensores para Atom Matrix
  GAM.begin(25,21);
  //Recogemos los valores actuales del acelerómetro que utilizaremos como referencia
  GAM.readA(dataA);

  for(byte c=0;c<2;c++) { //Recorremos las coordenadas X e Y
    refXY[c]=dataA[c]; //Anotamos el valor actual para este eje como referencia
    ballXY[c]=displaySize[c]/2; //Coordenadas iniciales de la bola (centro de display)
    ballSpeed[c]=0; //Inicialmente la bola no tiene velocidad
  }
}

void loop() {
  //Leemos los datos actuales del acelerómetro
  GAM.readA(dataA);
  //Recorremos las dos coordenadas
  for(byte c=0;c<2;c++) {
    //Calculamos la aceleración
    //Se la sumamos a la velocidad actual
    ballSpeed[c]+=((float)(dataA[c]-refXY[c]))/accel;
    //Calculamos la nueva coordenada
    ballXY[c]+=ballSpeed[c];
    //Si la bola está fuera de pantalla, debe rebotar con los bordes
    if(ballXY[c]<0) {
      ballXY[c]*=-elastic;
      ballSpeed[c]*=-elastic;
    } else if(ballXY[c]>displaySize[c]) {
      ballXY[c]=displaySize[c]-(ballXY[c]-displaySize[c])*elastic;
      ballSpeed[c]*=-elastic;
    }
  }
  //Borramos la memoria de vídeo
  display.v->clear();
  //Dibujamos sprite en las coordenadas calculadas (y invertida)
  display.v->drawPixel(ballXY[0],ballXY[1],{0,100,0}); //Verde
  //mostramos el resultado
  display.draw();
}
