/*
  Author: Ramón Junquera
  Tema: Rotary encoder
  Objetivo: Leer un dos rotary encoders KY-040 por interrupciones
  Fecha: 20180318  
  Material: breadboard, Arduino Mega, 2x rotary encoder Keyes KY-040, lector SD, display SSD1306
  Descripción:
    Utilizaremos dos rotary encoders.
    Lo gestionaremos por interrupciones.
    Los contadores de los rotary encoders se mostrarán en un display I2C.
    No utilizaremos los botones de los rotary encoders.
    En placas Arduino las interrupciones están ligadas a ciertos pines.
    Mega tiene 6 pines que soportan interrupciones.
    De los 6 pines, dos de ellos corresponden al protocolo I2C que también utilizamos en este
      ejemplo para la comunicación con el display, Por lo tanto, utilizaremos los otros 4.
    Puesto que queremos representar un valor numérico en el display y para
    ello necesitamos una fuente, estamos obligados a añadir al circuito un lector
    de tarjetas SD que contenga el archivo de la fuente.
    Usaremos la versión SD del driver.
  Resultado:
    Se muestran los contadores de los rotary encoders en el display
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
  //Fijamos los límites del contador de los rotary encoders
  encoder1.maxStepsCounter=10;
  encoder1.minStepsCounter=-10;
  encoder2.maxStepsCounter=10;
  encoder2.minStepsCounter=-10;
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
    
    //Actualizamos los valores en el display
    
    //Creamos el sprite que contendrá el texto
    RoJoSprite textSprite;
    //Creamos objeto de gestión de fuentes
    RoJoABC font;
    //Limpiamos la memoria de vídeo
    display.videoMem->clear();
    //Creamos el sprite de texto del rotary1
    font.print(F("/21x33d.fon"),String(previousStepsCounter1),&textSprite);
    //Dibujamos el sprite centrado
    display.videoMem->drawSpritePage((display.xMax-textSprite.width())/2,0,&textSprite,4);
    //Creamos el sprite de texto del rotary2
    font.print(F("/21x33d.fon"),String(previousStepsCounter2),&textSprite);
    //Dibujamos el sprite centrado
    display.videoMem->drawSpritePage((display.xMax-textSprite.width())/2,4,&textSprite,4);
    //Refrescamos pantalla
    display.show();
    //Borramos el sprite utilizado
    textSprite.clean();
  }
}
