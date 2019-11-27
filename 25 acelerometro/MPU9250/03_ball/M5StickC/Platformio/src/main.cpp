/*
  Autor: Ramón Junquera
  Fecha: 20191104
  Tema: Acelerómetro 
  Objetivo: Mostrar lecturas gráficamente
  Material: M5Stick-C

  Descripción:
    Mostraremos la imagen de una bola en el display y la posición del dispositivo se tendrá en cuenta en su
    movimiento.
    No ulizaremos el eje Z en este desarrollo.
    El sistema se calibra solo. Supone que la posición inicial es la de reposo.
    Se añade efecto rebote contra los límites del display para que sea más fácil controlar la bola y
    para que el comportamiento de la bola sea más realista.

  Resultado:
    Vemos que la bola del display responde a las inclinaciones del dispositivo
*/

#include <Arduino.h>
#include <RoJoST7735S.h> //Librería de gestión del display ST7735S
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoMPU9250.h> //Librería de gestión de sensores de MPU9250

//Definición de constantes globales
float displaySize[2]; //Área dibujable (display - tamaño sprite)
const float elastic=0.5; //Coeficiente de elasticidad. 0=no rebota. 1=siempre rebota
const float accel=2000; //Coeficiente de aceleración. Cuanto más pequeño, más rápido

//Definición de variables globales
RoJoST7735S display;
RoJoSprite ball; //Sprite color en movimiento
RoJoSprite v; //Sprite color de memoria de vídeo
RoJoMPU9250 GAM; //Sensores
int16_t dataA[3]; //Datos del acelerómetro
int16_t refXY[2]; //Valores de referencia
float ballXY[2]; //Coordenadas del sprite
float ballSpeed[2]; //Velocidad de la bola

void setup() {
  //Inicialización para M5Stick-C
  display.begin();
  //Definición del sprite de bola
  ball.setSize(5,5); //Lo dimensionamos. 5x5
  ball.circle(2,2,2,0xFFFF); //Círculo blanco
  //Definimos el área dibujable (display - tamaño de sprite de bola)
  displaySize[0]=display.xMax()-5;
  displaySize[1]=display.yMax()-5;
  //Dimensionamos sprite de memoria de vídeo
  v.setSize(display.xMax(),display.yMax());

  //Inicialización de sensores
  GAM.begin(21,22); //En M5Stack-C se deben indicar los pines
  //Recogemos los valores actuales del acelerómetro que utilizaremos como referencia
  GAM.readA(dataA);

  for(byte c=0;c<2;c++) { //Recorremos las coordenadas X e Y
    refXY[c]=-dataA[c]; //Anotamos el valor actual para este eje como referencia
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
    ballSpeed[c]+=((float)(refXY[c]-dataA[c]))/accel;
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
  v.clear();
  //Dibujamos sprite en las coordenadas calculadas (y invertida)
  v.drawSprite(&ball,ballXY[0],displaySize[1]-ballXY[1]);
  //mostramos el resultado
  display.drawSprite(&v);
}
