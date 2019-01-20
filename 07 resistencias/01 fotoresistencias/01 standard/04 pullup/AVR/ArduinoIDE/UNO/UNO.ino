/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia
  Objetivo: Funcionamiento de una fotorresistencia estándar
  Material: breadboard, fotorresistencia, cables, Arduino UNO

  Descripción:
  Tomando como base el ejercicio anterior en el que conectábamos la fotoresistencia a
  un pin digital, simplificaremos el circuito utilizando las resistencias internas
  de pull-up para no tener que ponerlas nosotros.
  La fotoresistencia se comporta como un interruptor de luz.

  Seguiremos utilizando el led integrado en placa para mostrar el estado del pin
  digital en el que tenemos conectada la fotoresistencia (12).

  Diferencias con el ejercicio anterior...
  Antes utilizábamos una resistencia de pull-down que nos mantenía el valor leído a
  estado LOW en el caso que la fotoresistencia no trabaje.
  Ahora es lo contrario. La resistencia interna de pull-up nos mantiene el pin
  digital a HIGH si la fotoresistencia no trabaja.
  Por eso podemos comprobar que si quitamos la fotoresistencia de la breadboard, 
  automáticamente se enciende el led integrado.
  Pero cuando está conectada se apaga, porque la fotoresistencia ofrece poca 
  resistencia y deja que GND se conecte con el pin digital.
  Cuando hay muy poca luz la resistencia se hace muy alta. Más alta que la 
  resistencia interna de pull-up, que es de 10 Kohmios. En ese momento se activa el
  estado HIGH en el pin digital y se enciende el led integrado.
 
  Resultado:
  El led integrado se mantiene apagado mientras haya suficiente luz.
*/

#include <Arduino.h>

void setup()
{
  //Configuramos el pin 13 como salida para poder escribir en él
  pinMode(13, OUTPUT);
   //Configuramos el pin de entrada con resistencia de PULLUP
  pinMode(12,INPUT_PULLUP);
}

void loop()
{
  //El estado del led integrado (13) será el mismo que el del pin digital 12
  digitalWrite(13,digitalRead(12));
}
