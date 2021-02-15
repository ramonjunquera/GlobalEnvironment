/*
  Autor: Ramón Junquera
  Tema: Bluetooth
  Fecha: 20180822
  Objetivo: Recepción contínua en un servidor (joy)
  Material: ESP32 OLED TTGO 16Mb

  Descripción:
  Para este ejemplo utilizaremos la aplicación Bluetooth Remote Joystick instalada en el móvil como cliente.
  Esta applicación permite utilizar el móvil como un joystick mostrando dos barras de desplazamiento
  perpendiculares, además de 3 botones.

  Nosotros sólo utilizaremos las mándos del joystick.
  
  En el servidor recogeremos los valores y haremos una representación gráfica.
  Para ello utilizaremos un ESP32 con una pantalla OLED monocroma integrada (ESP32 OLED TTGO 16Mb).
 
  ESP32 es suficientemente rápido como para recibir, parsear, calcular y dibujar recibiendo actualizaciones
  cada centésima de segundo (10 milisegundos).

  Resultado:
  Una bola dibujada en el display muestra la posición del joystick en tiempo real
*/

#include <Arduino.h>
#include "BluetoothSerial.h" //Gestión de Bluetooth
#include "RoJoSSD1306.h" //Librería de gestión del display SSD1306
#include "RoJoSprite.h" //Librería de gestión de sprites monocromos

//Constantes globales

//Variables globales
BluetoothSerial BT; //Objeto de gestión del bluetooth
RoJoSSD1306 display; //Objeto de gestión del display
RoJoSprite mySprite; //Objeto de gestión del sprite
String rxString=""; //Cadena de texto recibido
int y; //Última coordenada vertical

void setup()
{
  Serial.begin(115200);
  //Activamos el Bluetooth
  BT.begin("ESP32");
  //Inicializamos el display con los pines en los que está conectado la placa
  //En este modelo no son seleccionables y no coinciden con los estandar:
  //void begin(byte pinCS_SD,byte pinSDA,byte pinSCL,byte pinRST);
  //Nota es este ejemplo no se utiliza una tarjeta SD para almacenar los archivos
  //por lo tanto el pinCS_SD no se tiene en cuenta. Podríamos escribir cualquier valor
  display.begin(SS,4,15,16);

  //Dimensionamos el sprite. Anchura=4. Páginas=1
  mySprite.setSize(4,1);
  //Lo definimos
  //void setPage(int16_t x,int16_t page,byte mask,byte color);
  mySprite.drawPage(0,0,0b00000110,4); //4=escribir el valor tal cual
  mySprite.drawPage(1,0,0b00001111,4);
  mySprite.drawPage(2,0,0b00001111,4);
  mySprite.drawPage(3,0,0b00000110,4);
}

int8_t sign(float a)
{
  //Función que indica el signo de un valor float
  return (a>0)-(a<0); 
}

void loop()
{
  //Si hay algún dato recibido pendiente de ser procesado...
  if (BT.available())
  {
    //...lo leemos
    byte rxByte=BT.read();
    //Todas las cadenas de comandos que envía el programa finalizan con los caracteres 13 y 10
    //Las cedenas de los botones comienzan por B: B1, B2 o B3
    //Se envían cuando se suelta el botón
    //Las cadenas del joystick son de dos tipos. Las que comienzan por S (Speed) de la barra vertical
    //y las que comienzan por D (Direction) de la barra horizontal.
    //Después de cada prefijo vendrán 3 dígitos con el valor.
    //El valor se completará con ceros por la izquierda para que siempre tenga una longitud de 3 dígitos.
    //Los valores del joystick se envían contínuamente siempre que haya al menos una barra en una 
    //posición que no sea la central
    //Siempre se envía primero una cadena de S seguida por otra de D

    //Sabiendo esto, cuando recibamos un comando S, almacenaremos su valor
    //Y cuando recibamos un comando D, refrescaremos el display utilizando la coordenada vertical almacenada

    //Si el carácter recibido no es el 10 (fin de línea)...añadimos el carácter a la cadena
    if(rxByte!=10) rxString+=(char)rxByte;
    //Si hemos recibido el fin de línea...
    else
    {
      //Debemos parsear el texto recibido
      //Si comienza por un S...
      if(rxString[0]=='S')
      {
        //Comienza por el carácter esperado. Lo eliminamos
        rxString=rxString.substring(1);
        //Guardaremos el valor como coordenada vertical
        y=rxString.toInt();
      }
      //Si comienza por D...
      else if(rxString[0]=='D')
      {
        //Comienza por el carácter esperado. Lo eliminamos
        rxString=rxString.substring(1);
        //Guardaremos el valor como coordenada horizontal
        int x=rxString.toInt();
        //Hemos terminado de recibir ambas coordenadas

        //El rango de valores recibidos para ambas coordenadas es [0,510]
        //Y el valor por defecto (cuando no se toca la barra) es 255
        //En horizontal 0 está a la izquierda y 510 a la derecha
        //En vertical 0 está abajo y 510 arriba

        //Recordemos que la resolución del display es 128x64
        //y es sprite tiene un tamaño de 4x4
        //Por lo tanto el rango horizontal útil es [0,124]
        //y el vertical [0,60]

        //Para la coordenada horizontal debemos transformar el rango
        //[0,510] a [0,124]
        //x=x*124/510;
        x=(x*31)/130;

        //Para la coordenada horizontal debemos transformar el rango
        //[510,0] a [0,60]
        //y=(510-y)*60/510;
        y=(510-y)*2/17;

        //Dibujamos el sprite
        display.videoMem->clear();
        display.videoMem->drawSprite(x,y,&mySprite,1);
        display.show();
      }
      //Ya hemos procesado este string. Lo reseteamos
      rxString="";
    }
  }
}
