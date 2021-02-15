/*
  Author: Ramón Junquera
  Tema: Rotary encoder
  Objetivo: Mover un sprite por pantalla controlando sus coordenadas con dos rotary encoders gestionados por interrupciones
  Fecha: 20180318  
  Material: breadboard, Arduino Mega, 2x rotary encoder Keyes KY-040, display SSD1306
  Descripción:
    Utilizaremos dos rotary encoders.
    Lo gestionaremos por interrupciones.
    Se creará un sprite definido en tiempo de ejecución.
    La coordenadas del sprite serán contoladas por los rotary encoders.
    
    En placas Arduino las interrupciones están ligadas a ciertos pines.
    Mega tiene 6 pines que soportan interrupciones.
    De los 6 pines, dos de ellos corresponden al protocolo I2C que también utilizamos en este
      ejemplo para la comunicación con el display, Por lo tanto, utilizaremos los otros 4.
    Utilizaremos la versión del driver que utiliza un lector de tarjetas SD. Pero en este ejemplo no se necesita
      cargar nada desde la SD, por lo tanto el lector SD se ha eliminado del circuto.
      Simplemente se cargarán en memoria la librería SD pero nunca se hará uso de ella porque no usamos fuentes, ni
      leemos sprites, ni los guardamos.
  Resultado:
    Se muestra un sprite en el display que se controla por los rotary encoders.
  Notas:
    Este desarrollo no puede funcionar en placas más simples como UNO o Nano porque sólo tienen 2 interrupciones
*/

#include <Arduino.h>
#include "RoJoSSD1306SD.h" //Librería de gestión del display SSD1306
#include "RoJoSpriteSD.h" //Librería de gestión de sprites monocromos
#include "RoJoRotary040.h" //Gestión del rotary encoder

//Definición de constantes globales
const byte encoder1PinA = 2;
const byte encoder1PinAInt = 0;
const byte encoder1PinB = 3;
const byte encoder1PinBInt = 1;
const byte encoder2PinA = 19;
const byte encoder2PinAInt = 4;
const byte encoder2PinB = 18;
const byte encoder2PinBInt = 5;

//Definición global de variables
RoJoRotary040 encoder1 = RoJoRotary040(encoder1PinA,encoder1PinB);
RoJoRotary040 encoder2 = RoJoRotary040(encoder2PinA,encoder2PinB);
RoJoSSD1306 display; //Gestión del display
int previousStepsCounter1 = 9; //Lo fijamos a un valor distindo de 0 para que refresque al inicio
int previousStepsCounter2 = 9; //Lo fijamos a un valor distindo de 0 para que refresque al inicio
RoJoSprite mySprite;

void update1()
{
  //Función llamada cuando se detecta una variación en los estados de los pines de rotación del rotary1

  //Actualizamos el estado del rotary encoder
  encoder1.update();
}

void update2()
{
  //Función llamada cuando se detecta una variación en los estados de los pines de rotación del rotary2

  //Actualizamos el estado del rotary encoder
  encoder2.update();
}

void setup()
{
  //Inicializamos el display
  display.begin();
  //Dimensionamos el sprite. Anchura=7. Páginas=1
  mySprite.setSize(7,1);
  //Lo dibujamos
  //void setPage(int16_t x,int16_t page,byte mask,byte color);
  mySprite.drawPage(0,0,0b00111110,4); //4=escribir el valor tal cual
  mySprite.drawPage(1,0,0b01000001,4);
  mySprite.drawPage(2,0,0b01010101,4);
  mySprite.drawPage(3,0,0b01010001,4);
  mySprite.drawPage(4,0,0b01010101,4);
  mySprite.drawPage(5,0,0b01000001,4);
  mySprite.drawPage(6,0,0b00111110,4);
  //Limitamos el rango de valores de los encoders para que el sprite no se salga de pantalla
  encoder1.minStepsCounter=0;
  encoder1.maxStepsCounter=display.xMax-1-mySprite.width();
  encoder2.minStepsCounter=0;
  encoder2.maxStepsCounter=display.yMax-1-mySprite.heightPages()*8;
  //Creamos las interrupciones de los pines de rotación 
  attachInterrupt(encoder1PinAInt, update1, CHANGE);
  attachInterrupt(encoder1PinBInt, update1, CHANGE);
  attachInterrupt(encoder2PinAInt, update2, CHANGE);
  attachInterrupt(encoder2PinBInt, update2, CHANGE);
}

void loop()
{
  //Comprobamos contínuamente si coincide el valor del último contador anotado con el real
  //Si no es así, actualizamos los contadores en el display

  //Si no coincide alguno de los dos contadores con los últimos valores con los reales...
  if(previousStepsCounter1!=encoder1.stepsCounter || previousStepsCounter2!=encoder2.stepsCounter)
  {
    //Anotamos los valores actuales como últimos
    previousStepsCounter1=encoder1.stepsCounter;
    previousStepsCounter2=encoder2.stepsCounter;
    
    //Actualizamos la posición del sprite en el display
    //Limpiamos la memoria de vídeo
    display.videoMem->clear();
    //Dibujamos el sprite en la memoria de vídeo
    display.videoMem->drawSprite(previousStepsCounter1,previousStepsCounter2,&mySprite,1);
    //Refrescamos pantalla
    display.show();
  }
}
