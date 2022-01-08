/*
  Autor: Ramón Junquera
  Tema: Chip TM1637 / Display Catalex
  Objetivo: Aprender a utilizar el chip TM1637 integrado con un display de 4 dígitos tipo reloj
  Material adicional: breadboard, cables, display de led de 4 dígitos tipo reloj con chip TM1637

  Descripción:
  Aprenderemos a utilizar al chip TM1637 que viene integrado en el display de 4 dígitos de tipo
  reloj de la marca Catalex.
  
  Sólamente tiene 4 patas:
  - CLK clock
  - DIO data I/O
  - VCC 3.3V-5V
  - GND tierra
  
  Arduino no tiene una librería nativa para gestionar este chip, así que crearemos una para
  utilizarla en el ejemplo.
  La descripción detallada del funcionamiento del TM1637 se incluye dentro de los fuentes de
  la librería.
  
  Tal y como puede verse en la definición de variables globales, utilizamos los pines 2 y 3.
  No tenemos ningún otro elemento en el circuito.
  Mostraremos en el display el tiempo que ha transcurrido desde el último reinicio de la placa
  en formato MM:SS.
  Además haremos parpadear los dos puntos centrales cada medio segundo.

  Distribución de pines:
    CLK a pin 2
    DIO a pin 3
    VCC a 5V
    GND a GND

  Gestión de errores.
  Tanto la librería como el programa tienen en cuenta la gestión de errores. Si se produce
  cualquier error en cualquier comando puede ser detectado. En nuestro caso, cuando se detecta
  un error, se resetea el display. Con esto conseguimos que si alguna vez se envía algo con error,
  se mostrará el dígito mal escrito en el display, pero sólo durante un máximo de un segundo que
  es el tiempo de refresco.
  Los errores de comunicación más comunes son debidos a cableado deficiente:
  - El cable no hace buen contacto en le breadboard (cambia el agujero)
  - El display no está bien pinchado/conectado
  - El cable está roto y sólo hace contacto en cierta posición (cambia el cable)
  Si el circuto es estable y no da problemas, podríamos eliminar toda la gestión de errores
  del programa.

  En Fritzing no hay un componente, así que se incluye uno en la carpeta del proyecto.

  Resultado:
  Vemos el tiempo transcurrido en el display
*/

#include <Arduino.h>
#include "RoJoTM1637.h"

//Definimos constantes de pines
const byte pinCLK=D5;
const byte pinDIO=D7;

//Definimos objeto de gestión del display
RoJoTM1637 display;

//Tiempo en el que se debe hacer el próximo refresco de display
uint32_t nextTime=0; 

void setup()
{
  display.begin(pinCLK,pinDIO);
  display.setBrightness(4);
}

void loop()
{
  //Variable en la que tomamos nota de si ha ocurrido algún error
  bool withError;
  //Tomamos nota del tiempo transcurrido desde el último reset, en milisegundos
  uint32_t now=millis();
  
  //Si el tiempo que debemos mostrar supera los 100 minutos (100min * 60s/min * 1000ms/s)...
  if(now>6000000)
  {
    //...tiempo demasiado grande como para mostrar en una pantalla de 4 dígitos!
    
    //Mostramos los dos primeros guiones
    withError = display.set2DigitChar(0,17,17);
    //Mostramos los 2 segundos guiones 
    withError = (withError | display.set2DigitChar(2,17,17));
    //Si hemos tenido algún error escribiendo los guiones...
    if(withError)
    {
      //Reiniciamos el display
      display.enable();
    }
    else //No hemos tenido errores
    {
      //Esperamos medio segundo
      delay(500);
      //Borramos el display
      display.clear();
      //Esperamos otro medio segundo
      delay(500);
    }
  }
  else //El valor del tiempo actual se puede mostrar en el display
  {
    //Si hemos sobrepasado el tiempo límite para el próximo refresco...
    if(now>nextTime)
    {
      //...debemos refrescar el display
      //Si estamos en la primera mitad del segundo...
      if((now%1000)<500)
      {
        //Activamos los dos puntos
        withError = display.setDoubleDot(true);
        //Mostramos los minutos transcurridos a partir de la posición 0
        withError = withError | display.set2DigitValue(0,now/60000);
        //Mostramos los segundos transcurridos a partir de la posición 2      
        withError = withError | display.set2DigitValue(2,(now%60000)/1000);
        //Si hemos tenido algún error escribiendo el tiempo...
        if(withError)
        {
          //Reiniciamos el display
          display.enable();
        }
      }
      else //Si estamos en la segunda mitad del segundo...
      {
        //...desactivamos los dos puntos
        display.setDoubleDot(false);
      }
      //Lo refrescaremos de nuevo dentro de medio segundo (500ms)
      nextTime=now+500;      
    }
  }
}
