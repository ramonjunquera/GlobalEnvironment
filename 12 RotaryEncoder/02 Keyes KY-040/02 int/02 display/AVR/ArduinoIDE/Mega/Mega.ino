/*
  Author: Ramón Junquera
  Tema: Rotary encoder
  Objetivo: Leer un rotary encoder KY-040 por interrupciones
  Fecha: 20180318  
  Material: breadboard, Arduino Mega, rotary encoder Keyes KY-040, lector SD
  Descripción:
    Utilizaremos un rotary encoder.
    Lo gestionaremos por interrupciones.
    El contador del rotary encoder se mostrará en un display I2C.
    Al pulsar el botón se reseteará el contador.
    En placas Arduino las interrupciones están ligadas a ciertos pines.
    Mega tiene 6 pines que soportan interrupciones.
    Utilizaremos 3 de ellos. 2 para la rotación y uno para el pulsador.
    Puesto que queremos representar un valor numérico en el display y para
    ello necesitamos una fuente, estamos obligados a añadir al circuito un lector
    de tarjetas SD que contenga el archivo de la fuente.
    Usaremos la versión SD del driver.
  Resultado:
    Se muestra el contador del rotary encoder en el display
  Notas:
    No nos preocupamos por el pin CS de lector SD porque usamos el pin CS por defecto (SS=53)
    Este desarrollo no puede funcionar en placas más simples como UNO o Nano porque sólo tienen 2 interrupciones
*/

#include <Arduino.h>
#include "RoJoSSD1306SD.h" //Librería de gestión del display SSD1306
#include "RoJoSpriteSD.h" //Librería de gestión de sprites monocromos
#include "RoJoABCSD.h" //Gestión de fuentes
#include "RoJoRotary040.h" //Gestión del rotary encoder

//Definición de constantes globales
const byte encoderPinA = 18;
const byte encoderPinAInt = 5;
const byte encoderPinB = 2;
const byte encoderPinBInt = 0;
const byte encoderPinSwitch = 3;
const byte encoderPinSwitchInt = 1;

//Definición globar de variables
RoJoRotary040 encoder = RoJoRotary040(encoderPinA,encoderPinB);
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
  attachInterrupt(encoderPinAInt, update, CHANGE);
  attachInterrupt(encoderPinBInt, update, CHANGE);
  //Creamos la interrupción del pin del pulsador
  attachInterrupt(encoderPinSwitchInt, buttonPressed, FALLING);
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
