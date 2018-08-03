/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar la gestión de interrupciones
  Material adicional: breadboard, cables, interruptor de pulsador, placa Arduino UNO

  Descripción:
  La instrucción que nos permite relacionar una función con una interrupción es: attachInterrupt.
  La sintaxis correcta es:
    attachInterrupt(número de interrupción, función de interrupción,tipo de interrupción)
  Detallaremos cada uno de ellos:
    Número de interrupción.
      Es el número interno que identifica la interrupción que se gestiona.
      Depende de la placa.
      Estas son los códigos de interrupción y pines asociados de algunas placas:
      
      Arduino UNO/Nano
      pin interrupción
      --- ------------
       2   0
       3   1

      Arduino Mega
      pin interrupción
      --- ------------
       2   0
       3   1
      21   2
      20   3
      19   4
      18   5

    Función de interrupción.
      Es el nombre de una función que debemos tener definida, a la que se llamará cuando se
      de la situación de interrupción.
      La función no puede devolver valores ni puede tener parámetros.
    Tipos de interrupción.
      Es la situación que provoca la interrupción.
      No se pueden definir dos tipos de interrupción distintas para la misma interrupción. Sólo tomará
      la última. Por ejemplo, si queremos que salte la interrupción cuando su pin asociado pase de
      estado HIGH a LOW, y además queremos que salte otra interrupción cuando pase de LOW a HIGH, no
      tendremos más remedio que definir una sóla función que sea  llamada cuando cambie de estado sin
      importar a cual. Después ya gestionaremos el caso dentro de la misma función de interupción.
      Tenemos los siguientes tipos:
      - LOW. Cuando el pin está en estado LOW
      - CHANGE. Cuando el pin cambia de estado.
      - RISING. Cuando el pin pasa de estado LOW a HIGH
      - FALLING. Cuando el pin pasa de estado HiGH a LOW
      - HIGH. Cuando el pin está en estado HIGH. Atención este tipo sólo existe para Arduino Due.

  El comportamiento del flujo del programa dentro de una interrupción es distinto al habitual.
    - Dentro de una interrupción no se puede producir otra
    - Una interrupción debe ser lo más corta posible. Justo tomar nota de lo ocurrido o gestionar algo rápido.
    - Puesto que mientras se ejecuta una función de interrupción no se producen otras, todas aquellas
      funciones que se actualizan gracias a interrupciones quedan detenidas: millis(), micros(), delay(),
      gestión del puesto serie, ...
    - Como el puerto serie no funciona, no se puede recibir ni enviar nada dentro de una interrupción. Sí es
      verdad que el puerto serie tiene un buffer en el que sí se guardará el contenido a transmitir y será
      gestionado cuando se vuelvan a permitir de nuevo las interrupciones.
    - Que las funciones de tiempo (millis & micros) no funcionen, no significa que no podamos consultarlas.
      Simplemente no cambiarán su valor.
    - Podríamos tener problemas actualizando variables globales dentro de una función de interrupción.
      Para que esto no ocurra siempre definiremos estas variables globales como volátiles.
      Ej.: volatile bool currentStatus=false;
      Todas las variables que se modifican en una función de interrupción deben ser volátiles.

  Además de la instrucción attachInterrupt tenemos las siguientes funciones útiles para trabajar con interrupciones;:
    - noInterrupts() : Desactiva la ejecución de interrupciones definidas por el usuario (con attachInterrupt)
    - Interrupts() : Reactiva las interrupciones definidas por el usuario
    - detachInterrupt(número de interrupción) : Desactiva una interrupción concreta

  El ejemplo actual aplica el estado inverso del pin del interruptor al led.
  Es inverso porque hemos activado las resistencias de pullup del pin del interruptor y cuando no se pulsa
  su estado será HIGH.
  La diferencia con la técnica de polling es que no estamos contínuamente consultando el estado del interruptor.
  Sólo lo hacemos cuando tenemos la certeza que ha cambiado.
  Esto no permite dedicar el tiempo de proceso a otras tareas.

  Nota:
  Si utilizamos una placa MakerUNO no necesitaremos un interruptor externo. La se incluye uno integrado
  asociado al pin 2.

  Resultado:
  Cuando pulsamos el interruptor, se enciende el led integrado.
*/

#include <Arduino.h>

//Declaración de variables globales
const byte pinLed = 13; //Pin del led integrado
const byte pinSwitch = 2; //pin al que está conectado el interruptor
const byte intSwitch = 0; //Interrupción correspondiente al pin del interruptor

void IntSwitch()
{
  //Función a la que se llama cuando cambia el estado del interruptor

  //Aplicamos al led el estado inverso al del interruptor
  digitalWrite(pinLed,!digitalRead(pinSwitch));
}

void setup()
{
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Activamos las resistencias de pull-up para el pin de interruptor
  //Por defecto el pin del interruptor estará en HIGH y cuando se pulse en LOW
  pinMode(pinSwitch,INPUT_PULLUP);
  //Definimos la interrupción
  //Será la correspondiente al pin 2
  //Se llamará a la función IntSwitch
  //Ocurrirá cuando el estado del pin cambie
  attachInterrupt(intSwitch,IntSwitch,CHANGE);
}

void loop()
{
  //nada especial que hacer aquí
}



