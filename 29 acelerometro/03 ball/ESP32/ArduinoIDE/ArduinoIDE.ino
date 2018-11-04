/*
  Autor: Ramón Junquera
  Fecha: 20181104
  Tema: Acelerómetro GY-61
  Objetivo: Mostrar lecturas gráficamente
  Material: breadboard, acelerómetro GY-61, placa ESP32 OLED

  Descripción:
  Mostraremos la imagen de una bola en el display y el posición del dispositivo se tendrá en cuenta en su
  movimiento.
  No ulizaremos el eje Z en este desarrollo.
  El sistema se calibra solo. Supone que la posición inicial es la de reposo.
  Se añade efecto rebote contra los límites del display para que sea más fácil controlar la bola y
  para que el comportamiento de la bola sea más realista.

  Resultado:
  Vemos que la bola del display responde a las inclinaciones del dispositivo
*/

#include <Arduino.h>
#include "RoJoSSD1306.h" //Librería de gestión del display SSD1306
#include "RoJoSprite.h" //Librería de gestión de sprites monocromos

//Definición de constantes globales
const byte pins[]={36,37}; //Corresponden a los pinres A0 y A1
const float displaySize[]={124,60}; //Tamaño de display (restando tamaño de sprite)
const float elastic=0.5; //Coeficiente de elasticidad

//Definición de variables globales
RoJoSSD1306 display;
RoJoSprite ball;
float analogMid[2]; //Cálculo del valor medio
float analogMin[2]; //Mínimo valor admitido
float analogMax[2]; //Máximo valor admitido
float ballXY[2]; //Coordenadas del sprite
float value; //Valor leido
float ballSpeed[2]; //Velocidad de la bola

void setup()
{
  //Recorremos las coordenadas
  for(byte c=0;c<2;c++)
  {
    //Obtenemos valor analógico medio
    //Suponemos que está en horizontal
    analogMid[c]=analogRead(pins[c]);
    //Calculamos loa valores mínimo y máximo
    analogMin[c]=analogMid[c]-300;
    analogMax[c]=analogMid[c]+300;
    //Coordenadas iniciales de la bola (centro de display)
    ballXY[c]=displaySize[c]/2;
    //Inicialmente la bola no tiene velocidad
    ballSpeed[c]=0;
  }
  //Definición del sprite de bola
  ball.setSize(4,1); //Lo dimensionamos. Anchura=4. Páginas=1
  ball.drawPage(0,0,0b00000110,4); //4=escribir el valor tal cual
  ball.drawPage(1,0,0b00001001,4);
  ball.drawPage(2,0,0b00001001,4);
  ball.drawPage(3,0,0b00000110,4);
  //Inicialización del display
  display.begin(SS,4,15,16);
}

void loop()
{
  //Recorremos las dos coordenadas
  for(byte c=0;c<2;c++)
  {
    //Leemos la coordenada
    value=analogRead(pins[c]);
    //La normalizamos (no puede quedar fuera de los límites
    if(value<analogMin[c]) value=analogMin[c];
    if(value>analogMax[c]) value=analogMax[c];
    //Calculamos la aceleración
    value=(value-analogMid[c])/300*0.0005;
    //Variamos la velocidad
    ballSpeed[c]+=value;
    //Calculamos la nueva coordenada
    ballXY[c]+=ballSpeed[c];
    //Si la bola está fuera de pantalla, debe rebotar con los bordes
    if(ballXY[c]<0)
    {
      ballXY[c]*=-elastic;
      ballSpeed[c]*=-elastic;
    }
    else if(ballXY[c]>displaySize[c])
    {
      ballXY[c]=displaySize[c]-(ballXY[c]-displaySize[c])*elastic;
      ballSpeed[c]*=-elastic;
    }
  }
  //Dibujamos sprite en las coordenadas calculadas
  display.videoMem->clear();
  //El eje X está invertido
  display.videoMem->drawSprite(displaySize[0]-ballXY[0],ballXY[1],&ball,1);
  display.show();
}
