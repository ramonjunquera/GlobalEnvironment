/*
  Autor: Ramón Junquera
  Fecha: 20200218
  Tema: Acelerómetro 
  Objetivo: Mostrar lecturas gráficamente. Gestión de partículas
  Material: M5Stack Watch

  Descripción:
    Crearemos un número de partículas representadas por puntos.
    Cada una tendrá su posición y velocidad.
    Las lecturas del acelerómetro les afectará.
    Las partículas pueden chocar emtre ellas.
    Existe el efecto rebote.
    Se añaden dos objetos fijos en pantalla.
    Se varía ligeramente la aceleración de cada partícula para simular un plano irregular
    El tiempo aproximado del reloj de arena es de un minuto
*/

#include <Arduino.h>
#include <RoJoSH1107.h> //Librería de gestión del display SH1107
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos
#include <RoJoMPU9250.h> //Librería de gestión de sensores de MPU9250

//Definición de variables globales
RoJoSH1107 display;
RoJoSprite v(1); //Sprite monocromo de memoria de vídeo
RoJoMPU9250 GAM; //Sensores

const uint16_t particles=900; //Número de partículas
float coorXY[particles][2],speedXY[particles][2]; //Tablas de coordenadas y velocidad de cada partícula
int16_t dataA[3]; //Datos del acelerómetro
int16_t refXY[2]; //Valores de referencia del Acelerómetro
const float elastic=0.2; //Coeficiente de elasticidad. 0=no rebota. 1=siempre rebota
const long accelK=200000; //Coeficiente de aceleración. Cuanto más pequeño, más rápido

float accel[2]; //Aceleración del ciclo para cada eje

void setup() {
  //Serial.begin(115200); //DEBUG
  //Inicialización de display para M5 Watch
  display.begin(33,27,14);
  //Dimensionamos sprite de memoria de vídeo
  v.setSize(display.xMax(),display.yMax());
  //Todas las partículas aparecerán en las mismas coordenadas
  for(uint16_t p=0;p<particles;p++) {
    coorXY[p][0]=32;
    coorXY[p][1]=10;
  }
  //Añadimos un par de objetos inmóviles
  v.circle(-32-1,64,64,{0,0,1});
  v.circle(64+32+1,64,64,{0,0,1});

  //Inicialización de sensores
  GAM.begin();
  //Recogemos los valores actuales del acelerómetro que utilizaremos como referencia
  GAM.readA(refXY);
}

void loop() {
  //Leemos los datos actuales del acelerómetro
  GAM.readA(dataA);
  //Calculamos aceleración para cada eje
  //Variamos la constante de aceleración un máximo de un 5% en cada cálculo
  for(byte xy=0;xy<2;xy++) accel[xy]=((float)(dataA[xy]-refXY[xy]))/accelK;

  //Recorremos todas las partículas
  for(uint16_t p=0;p<particles;p++) {
    int8_t p1[2]; //Pixel origen
    int8_t p2[2]; //Pixel destino
    for(byte xy=0;xy<2;xy++) { //Recorremos los dos ejes
      p1[xy]=coorXY[p][xy]; //Calculamos las coordenadas del pixel origen
      //Sumamos la aceleración a la velocidad de la partícula
      //Añadimos un sumando aleatorio
      speedXY[p][xy]+=accel[xy]+((float)random(-100,100)/10000);
      p2[xy]=coorXY[p][xy]+speedXY[p][xy]; //Calculamos las coordenadas del pixel destino
    }

    //Si la coordenada horizontal está fuera de pantalla...
    if(p2[0]<0 || p2[0]>=display.xMax()) {
      //..cambiamos la velocidad horizontal
      speedXY[p][0]=-speedXY[p][0]*elastic;
    } else { //La coordenada horizontal está en pantalla
      //Si el pixel origen y destino es el mismo...
      if(p1[0]==p2[0]) {
        //Actualizamos las coordenadas flotantes
        coorXY[p][0]+=speedXY[p][0];
      } else { //El pixel origen y destino son distintos
        //Si el pixel destino está ocupado...
        if(v.getPixel(p2[0],display.yMax()-1-p1[1]).get24()>0) {
          //...ha chocado. Cambiamos la velocidad horizontal
          speedXY[p][0]=-speedXY[p][0]*elastic;
        } else { //Si el pixel destino está libre...
          //Borramos la partícula en origen
          v.drawPixel(p1[0],display.yMax()-1-p1[1],{0,0,0});
          //Movemos la coordenada horizontal
          coorXY[p][0]+=speedXY[p][0];
          p1[0]=p2[0];
          //Dibujamos el pixel de la nueva posición
          v.drawPixel(p1[0],display.yMax()-1-p1[1],{0,0,1});
        }
      }
    }

    //Si la coordenada vertical está fuera de pantalla...
    if(p2[1]<0 || p2[1]>=display.yMax()) {
      //..cambiamos la velocidad vertical
      speedXY[p][1]=-speedXY[p][1]*elastic;
    } else { //La coordenada vertical está en pantalla
      //Si el pixel origen y destino es el mismo...
      if(p1[1]==p2[1]) {
        //Actualizamos las coordenadas flotantes
        coorXY[p][1]+=speedXY[p][1];
      } else { //El pixel origen y destino son distintos
        //Si el pixel destino está ocupado...
        if(v.getPixel(p1[0],display.yMax()-1-p2[1]).get24()>0) {
          //...ha chocado. Cambiamos la velocidad vertical
          speedXY[p][1]=-speedXY[p][1]*elastic;
        } else { //Si el pixel destino está libre...
          //Borramos la partícula en origen
          v.drawPixel(p1[0],display.yMax()-1-p1[1],{0,0,0});
          //Movemos la coordenada vertical
          coorXY[p][1]+=speedXY[p][1];
          p1[1]=p2[1];
          //Dibujamos el pixel de la nueva posición
          v.drawPixel(p1[0],display.yMax()-1-p1[1],{0,0,1});
        }
      }
    }
  }
  //Mostramos la memoria de vídeo
  display.drawSprite(&v);
}
