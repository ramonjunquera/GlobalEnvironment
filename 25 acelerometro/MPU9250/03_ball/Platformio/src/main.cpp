/*
  Autor: Ramón Junquera
  Fecha: 20190916
  Tema: Acelerómetro 
  Objetivo: Mostrar lecturas gráficamente
  Material: M5Stack Watch

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
#include <RoJoSH1107.h> //Librería de gestión del display SH1107
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoMPU9250.h> //Librería de gestión de sensores de MPU9250

//Definición de constantes globales
const float displaySize[]={60,124}; //Tamaño de display (restando tamaño de sprite)
const float elastic=0.5; //Coeficiente de elasticidad. 0=no rebota. 1=siempre rebota
const float accel=200000; //Coeficiente de aceleración. Cuanto más pequeño, más rápido

//Definición de variables globales
RoJoSH1107 display;
RoJoSprite ball(1); //Sprite monocromo en movimiento
RoJoSprite v(1); //Sprite monocromo de memoria de vídeo
RoJoMPU9250 GAM; //Sensores
int16_t dataA[3]; //Datos del acelerómetro
int16_t refXY[2]; //Valores de referencia
float ballXY[2]; //Coordenadas del sprite
float ballSpeed[2]; //Velocidad de la bola

void setup() {
  //Definición del sprite de bola
  ball.setSize(4,1); //Lo dimensionamos. Anchura=4. Páginas=1
  ball.drawPage(0,0,0b00000110,4); //4=escribir el valor tal cual
  ball.drawPage(1,0,0b00001001,4);
  ball.drawPage(2,0,0b00001001,4);
  ball.drawPage(3,0,0b00000110,4);
  //Inicialización para M5 Watch
  //void begin(byte pinRES,byte pinDC,byte pinCS,uint32_t freqCOMM=0);
  display.begin(33,27,14);
  //Dimensionamos sprite de memoria de vídeo
  v.setSize(display.xMax(),display.yMax());

  //Inicialización de sensores
  GAM.begin();
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
  v.clear();
  //Dibujamos sprite en las coordenadas calculadas (y invertida)
  v.drawSprite(&ball,ballXY[0],displaySize[1]-ballXY[1]);
  //mostramos el resultado
  display.drawSprite(&v);
}
