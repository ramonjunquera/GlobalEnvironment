/*
  Autor: Ramón Junquera
  Fecha: 20200218
  Tema: Acelerómetro GY-61
  Objetivo: Mostrar lecturas gráficamente
  Material: breadboard, acelerómetro GY-61, placa ESP32 OLED

  Descripción:
    Mostraremos la imagen de una bola en el display y el posición del dispositivo
    se tendrá en cuenta en su movimiento.
    No ulizaremos el eje Z en este desarrollo.
    El sistema se calibra solo. Supone que la posición inicial es la de reposo.
    Se añade efecto rebote contra los límites del display para que sea más fácil
    controlar la bola y para que el comportamiento de la bola sea más realista.

  Resultado:
    Vemos que la bola del display responde a las inclinaciones del dispositivo
*/

#include <Arduino.h>
#include <RoJoSSD1306.h> //Librería de gestión del display SSD1306
#include <RoJoSprite.h> //Librería de gestión de sprites monocromos

//Definición de constantes globales
const byte pins[]={36,37}; //Corresponden a los pinres A0 y A1
const float displaySize[]={124,60}; //Tamaño de display (restando tamaño de sprite)
const float elastic=0.5; //Coeficiente de elasticidad. 0=no rebota. 1=siempre rebota
const float accel=700000; //Coeficiente de aceleración. Cuanto más pequeño, más rápido

//Definición de variables globales
RoJoSSD1306 display;
RoJoSprite ball;
RoJoSprite v(1); //Sprite monocromo de memoria de vídeo
float analogMid[2]; //Cálculo del valor medio
float ballXY[2]; //Coordenadas del sprite
float ballSpeed[2]; //Velocidad de la bola

void setup() {
  //Recorremos las coordenadas X e Y
  for(byte c=0;c<2;c++) {
    //Obtenemos valor analógico medio, suponiendo que está en horizontal
    analogMid[c]=analogRead(pins[c]);
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
  //Inicializamos el display con los pines en los que está conectado la placa
  //void begin(byte pinSDA,byte pinSCL,byte pinRST);
  display.begin(4,15,16);
  //Dimensionamos el sprite que usaremos como memoria de vídeo con el mismo
  //tamaño que el display
  v.setSize(display.xMax(),display.yMax());
}

void loop() {
  //Recorremos las dos coordenadas
  for(byte c=0;c<2;c++) {
    //Leemos la coordenada del pin analógico
    //Calculamos la aceleración
    //Se la sumamos a la velocidad actual
    ballSpeed[c]+=(analogRead(pins[c])-analogMid[c])/accel;
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
  v.clear(); //Borramos la memoria de vídeo
  //Dibujamos sprite en las coordenadas calculadas
  //El eje X está invertido
  v.drawSprite(&ball,displaySize[0]-ballXY[0],ballXY[1]);
  display.drawSprite(&v); //Lo mostramos
}