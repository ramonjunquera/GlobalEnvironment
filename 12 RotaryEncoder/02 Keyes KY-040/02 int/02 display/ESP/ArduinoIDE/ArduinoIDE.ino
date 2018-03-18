/*
  Author: Ramón Junquera
  Tema: Rotary encoder
  Objetivo: Leer un rotary encoder KY-040 por interrupciones
  Fecha: 20180318  
  Material: breadboard, ESP board, rotary encoder Keyes KY-040
  Descripción:
    Utilizaremos un rotary encoder.
    Lo gestionaremos por interrupciones.
    El contador del rotary encoder se mostrará en un display I2C.
    Al pulsar el botón se reseteará el contador.
  Resultado:
    Se muestra el contador del rotary encoder en el display
*/

#include <Arduino.h>
#include "RoJoSSD1306.h" //Librería de gestión del display SSD1306
#include "RoJoSprite.h" //Librería de gestión de sprites monocromos
#include "RoJoABC.h" //Gestión de fuentes
#include "RoJoRotary040.h" //Gestión del rotary encoder

//Definición de constantes globales
#ifdef ESP8266
  const byte encoderPinA = D5;
  const byte encoderPinB = D6;
  const byte encoderPinSwitch = D3;
#elif defined(ESP32)
  const byte encoderPinA = 15;
  const byte encoderPinB = 4;
  const byte encoderPinSwitch = 5;
#endif

//Definición globar de variables
RoJoRotary040 encoder = RoJoRotary040(encoderPinA,encoderPinB); //Gestión del rotary encoder
RoJoSSD1306 display; //Gestión del display
int previousStepsCounter = 9; //Lo fijamos a un valor distindo de 0 para que refresque al inicio

void update()
{
  //Función llamada cuando se detecta una variación en los estados de los pines de rotación

  //Actualizamos el estado del rotary encoder
  encoder.update();
}

void buttonPressed()
{
  //Función llamada cuando se presiona el botón del rotary encoder

  //Reseteamos el contador del rotary encoder
  encoder.stepsCounter = 0;
}

void setup()
{
  //Inicializamos el display
  display.begin();
  //Configuramos el pulsador del rotary encoder como entrada con las resistencias de pullup activadas
  pinMode(encoderPinSwitch, INPUT_PULLUP);
  //Fijamos los límites del contrador del rotary encoder
  encoder.maxStepsCounter=10;
  encoder.minStepsCounter=-10;
  //Creamos las interrupciones de los pines de rotación 
  attachInterrupt(encoderPinA, update, CHANGE);
  attachInterrupt(encoderPinB, update, CHANGE);
  //Creamos la interrupción del pin del pulsador
  attachInterrupt(encoderPinSwitch, buttonPressed, FALLING);
}

void loop()
{
  //Comprobamos contínuamente si coincide el valor del último contador anotado con el real
  //Si no es así, actualizamos el contador en el display

  //Si no coincide el último valor del contador con el real...
  if(previousStepsCounter!=encoder.stepsCounter)
  {
    //Anotamos el valor actual como último
    previousStepsCounter=encoder.stepsCounter;
    //Actualizamos el valor en el display
    
    //Creamos el sprite que contendrá el texto
    RoJoSprite textSprite;
    //Creamos objeto de gestión de fuentes
    RoJoABC font;
    //Creamos el sprite de texto
    font.print(F("/21x33d.fon"),String(previousStepsCounter),&textSprite);
    //Limpiamos la memoria de vídeo
    display.videoMem->clear();
    //Dibujamos el sprite centrado
    display.videoMem->drawSpritePage((display.xMax-textSprite.width())/2,2,&textSprite,4);
    //Refrescamos pantalla
    display.show();
    //Borramos el sprite utilizado
    textSprite.clean();
  }
}
