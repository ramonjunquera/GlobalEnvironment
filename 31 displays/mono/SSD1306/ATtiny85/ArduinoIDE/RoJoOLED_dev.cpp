 /*
  Nombre de la librería: RoJoOLED.h para ATtiny85
  Autor: Ramón Junquera
  Fecha: 20191029
  Descripción:
    Gestión de display OLED I2C 0.96" 128x64 SSD1306
*/

#include <Arduino.h>
#include <TinyWireM.h> //Gestión de comunicaciones I2C para ATtiny85

#include "RoJoOLED_dev.h"


//Define  rango X & Y
void RoJoOLED::_setCursorRange(byte x1,byte page1,byte x2,byte page2) {
  TinyWireM.beginTransmission(_oledId); //Abrimos conexión con el dispositivo
  TinyWireM.send(0); //Indicamos que a continuación se enviarán comandos

  TinyWireM.send(0x22); //Comando para definir el rango de páginas que utilizaremos : [0,7]
  TinyWireM.send(page1); //Página inicial
  TinyWireM.send(page2); //Página final

  TinyWireM.send(0x21); //Comando para definir el rango de columnas que utilizaremos: [0,127]
  TinyWireM.send(x1); //Columna inicial
  TinyWireM.send(x2); //Columna final

  TinyWireM.endTransmission(); //Hemos terminado de enviar comandos
}

//Dibuja una línea vertical
void RoJoOLED::lineV(byte x) {
  _setCursorRange(x,0,x,7); //Definimos el rango de la línea
  TinyWireM.beginTransmission(_oledId); //Abrimos comunicación con el display
    TinyWireM.send(0x40); //Indicamos que a continuación se enviarán datos
    for(byte p=0;p<8;p++) //Recorremos todas las páginas
      TinyWireM.send(0xFF);
  TinyWireM.endTransmission(); //Hemos terminado de enviar datos
}

//Borra el área de dibujo
void RoJoOLED::clear(byte value) {
  //Rellenaremos toda la memoria gráfica de ceros

  _setCursorRange(); //Definimos todo el display como área de dibujo
  for(byte p=0;p<8;p++) //Recorremos todas las páginas
    for(byte b=0;b<8;b++) { //Recorremos los bloques de 16 bytes de una página
      TinyWireM.beginTransmission(_oledId); //Abrimos comunicación con el display
      TinyWireM.send(0x40); //Indicamos que a continuación se enviarán datos
      //Recorremos los 16 bytes de un bloque y enviamos el dato al display
      for(byte d=0;d<16;d++) TinyWireM.send(value);
      TinyWireM.endTransmission(); //Hemos terminado de enviar datos
    }      
}

//Comprueba la conexión con el display y lo inicializa
//Devuelve true si lo consigue
bool RoJoOLED::begin() {
  //Activamos la conexión I2C
  TinyWireM.begin();

  //Abrimos comunicación con el display
  TinyWireM.beginTransmission(_oledId); 
  //Se solicita finalizar la comunicación. Si ocurre algún error es que no se pudo encontrar el dispositivo
  if (TinyWireM.endTransmission() != 0) return false;
  //El dispositivo se ha encontrado

  //El display tiene una limitación de 16 bytes por cada transmisión.
  //Para inicializar necesitamos enviar más de 16 bytes
  //Para no tener problemas, lo haremos en dos conexiones distintas

  TinyWireM.beginTransmission(_oledId); //Abrimos conexión con el dispositivo
  TinyWireM.send(0); //Indicamos que a continuación se enviarán comandos

  TinyWireM.send(0x8D); //Rango de voltaje
  TinyWireM.send(0x14); //Voltaje superior a 3.3V

  TinyWireM.send(0x20); //Orden de escritura gráfica
  TinyWireM.send(0x00); //Modo horizontal

  TinyWireM.send(0xC8); //Orientación vertical con los pines de conexión en la parte superior
  TinyWireM.send(0xA1); //Orientación horizontal. De izquierda a derecha 0-->128

  TinyWireM.endTransmission(); //Hemos terminado de enviar comandos
  TinyWireM.beginTransmission(_oledId); //Abrimos conexión con el dispositivo
  TinyWireM.send(0); //Indicamos que a continuación se enviarán comandos

  TinyWireM.send(0xDA); //Filas entrelazadas
  TinyWireM.send(0x10); //Modo no entrelazado

  TinyWireM.send(0x21); //Definimos el rango de columnas que utilizaremos: [0,127]
  TinyWireM.send(0x00); //Desde la 0
  TinyWireM.send(0x7F); //Hasta la 127
  
  TinyWireM.send(0x22); //Definimos el rango de páginas que utilizaremos : [0,7]
  TinyWireM.send(0x00); //Desde la 0
  TinyWireM.send(0x07); //Hasta la 7

  TinyWireM.send(0x81); //Ajustamos el contraste
  TinyWireM.send(0x00); //Al mínimo

  TinyWireM.send(0xAF); //Encendemos el display

  TinyWireM.endTransmission(); //Hemos terminado de enviar comandos
  
  return true; //Todo correcto
}
