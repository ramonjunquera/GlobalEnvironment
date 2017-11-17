/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar la gestión de interrupciones de tiempo de Arduino
  Material: placa Arduino

  Descripción:
  En este ejercicio aprenderemos a definir una clase que gestione las interrupciones.
  Definiremos una clase que haga parpadear un pin determinado, en el que deberíamos poner un led para poder
  ver el resultado.
  El periodo de parpadeo será fijo de un segundo.

  Como siempre, utilizaremos el timer 1.
  La interrupción saltará cuando el contador llegue al máximo que soporta el timer (65535).
  Recordando los periodos que se obtienen dependiendo del prescaler...

  prescaler=1    periodo=prescaler*MaxValue/frecuencia=1*65535/16000000=0,0040959375s=4,0959375ms~4,1ms
  prescaler=8    periodo=prescaler*MaxValue/frecuencia=8*65535/16000000=0,0327675s=32,7675ms~33ms
  prescaler=64   periodo=prescaler*MaxValue/frecuencia=64*65535/16000000=0,26214s~0,3s
  prescaler=256  periodo=prescaler*MaxValue/frecuencia=256*65535/16000000=1,04856s~1s
  prescaler=1024 periodo=prescaler*MaxValue/frecuencia=1024*65535/16000000=4,19424s~4,2s

  Tomaremos el prescaler de 256 que ya nos da un periodo de un segundo.

  Desde le programa principal, lo único que hacemos es referenciar al arhivo de cabecera Timer1TestClass.h
  Sólo con esto, se crea una variable global con una instancia de la clase.
  En la función setup() utilizamos el método pinLed() para definir el pin afectado.
  El resto del funcionamiento es automático y se encargará la clase de gestionarlo.

  Los comentarios de la clase se incluyen dentro de los archivos de la clase.

  Resultado:
  El led integrado en placa parpadea cada segundo.
*/

#include <Arduino.h>
//Incluimos la clase. Automáticamente se crea el objeto timerClass referenciando a la clase
#include "Timer1TestClass.h"

void setup()
{
  //Indicamos que queremos que parpadee el pin 13
  timerClass.pinLed(13);
}

void loop()
{
  //No tenemos nada que ejecutar en el bucle principal
}



