/*
  Autor: Ramón Junquera
  Tema: Timer - Ticker
  Fecha: 20180621
  Objetivo: Ticker. Función con parámetros
  Material: placa WeMos D1, interruptor

  Descripción:
  En ejercicios anteriores hemos utilizado el objeto Ticker para llamar a funciones que no
  tenían parámetros.
  Ahora aprenderemos a pasar un parámetro en la llamada.

  Existen limitaciones en las llamadas a funciones con argumentos:
  - Sólo se admite un argumento
  - El argumento tiene un tamaño máximo de 4 bytes. Esto significa que sólo son reconocidos los
    siguientes tipos: byte, char, short, int, float, void*, char*

  Antes de continuar, haremos un repaso de los métodos públicos disponibles en la clase Ticker:
    void attach(float seconds, callback_t callback)
      Asigna una función sin parámetros para que se llame cada ciertos segundos
    void attach(float seconds, void (*callback)(TArg), TArg arg)
      Asigna una función con un parámetro para que se llame cada ciertos segundos
    void attach_ms(uint32_t milliseconds, callback_t callback)
      Asigna una función sin parámetros para que se llame cada ciertos milisegundos
    void attach_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
      Asigna una función con un parámetro para que se llame cada ciertos milisegundos
    void once(float seconds, callback_t callback)
      Asigna una función sin parámetros que se llamará una sóla vez después de los segundos indicados
    void once_ms(uint32_t milliseconds, callback_t callback)
      Asigna una función sin parámetros que se llamará una sóla vez después de los milisegundos indicados
    void once(float seconds, void (*callback)(TArg), TArg arg)
      Asigna una función con un parámetro que se llamará una sóla vez después de los segundos indicados
    void once_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
      Asigna una función con un parámetro que se llamará una sóla vez después de los milisegundos indicados
    void detach()
      Desactiva las llamadas a la función asignada
      
  Aprovechando que WeMos D1 tiene dos leds integrados, crearemos dos objetos Ticker.
  Uno para cada led. Con distintos periodos.
  Pero sólo definiremos una única función que cambiará el estado del pin indicado
  como parámetro. Por lo tanto, los Tickers llamarán a una sóla función pero cada
  uno con el parámetro del pin que le corresponde.

  Nota:
  El objeto Ticker se basa en timers internos propios del procesador.
  Cuando se llama a la función indicada es porque ha saltado una interrupción por
  software.
  Mientras se ejecuta la función, no hay más interrupciones de timers.
  La función delay o millis funcionan gracias a timers.
  Esto quiere decir que delay y millis no funcionarán dentro de una
  función a la que llama Ticker, porque no refrescarán nunca su valor.
  Conclusión: no se pude utilizar delay en una función llamada por un timer.

  Nota:
  La primera (o única llamada) a la función se produce transcurrido el tiempo de periodo
  indicado.

  Resultado:
  Los dos leds integrados parpadean, cada uno a su propia frecuencia.
*/

#include <Arduino.h>
#include <Ticker.h> //Gestión de timers

//Creamos un objeto Ticker para cada pin
//El pin D4 está asociado al led integrado de antena
//El pin D5 está asociado al led integrado SCK
Ticker blinkerD4, blinkerD5;

void setup() 
{
  //Configuramos los pines de los leds integrados como salida
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  //Asignamos la función Blink a los objetos Ticker cada uno con su frecuencia
  blinkerD4.attach_ms(400,Blink,D4);
  blinkerD5.attach_ms(500,Blink,D5);
}
void loop() 
{
  //No tenemos nada especial que hacer en la función loop()
}

void Blink(byte pin)
{
  //Cambia el estado del pin indicado
  digitalWrite(pin,!digitalRead(pin));
}
