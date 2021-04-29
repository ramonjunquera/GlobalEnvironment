/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Fecha: 20181010
  Objetivo: Aprender a utilizar la gestión de interrupciones de tiempo de Arduino
  Material: placa Arduino

  Descripción:
  Continuamos con la programación de los timers.
  En el ejercicio anterior hemos aprendido a configurar el timer1 para que produzca interrupciones en un tiempo
  definido por nosotros.
  Este tiempo siempre está limitado por el tiempo máximo que puede esperar el timer1, que ya calculamos que es de 4,2s.
  Pero qué pasa si necesitamos que el timer funcione con menor frecuencia?. Por ejemplo 10 segundos?

  Este ejercicio es una modificación del anterior.
  Dejaremos el periodo del timer a un segundo, pero añadiremos nosotros un contador adicional que se encargará de "contar"
  el número de interrupciones que van saltando (myCTC). Como se produce cada segundo, nuestro contador será el número de
  segundos que transcurren entre la acción del timer.
  En la función a la que llama el timer nos encargaremos de aumentar nuestro contador y compararlo con nuestro límite (myCTCmax).

  Definimos la variable global myCTC como volatile para que vuando modifiquemos su valor dentro de la función de interrupción
  no afecte al resto de programa.
    
  Resultado:
  El led integrado en placa parpadea cada 10 segundos.
*/

#include <Arduino.h>

//Como trabajaremos directamente con los registros del procesador, incluiremos las librerías de gestión del fabricante
//del procesador. No hacemos referencia a librerías de Arduino.
#include <avr/io.h>
#include <avr/interrupt.h>

//Declaración de variables globales
const byte pinLed = 13; //Pin al que está conectado el led
const byte myCTCmax = 10; //Número de segundos (interrupciones) que deben transcurrir para que se ejecute la acción del timer

void setup()
{
  //Configuramos el pin del led para que sea de salida
  pinMode(pinLed,OUTPUT);
  //Deshabilitamos las interrupciones globales. No queremos que salte ninguna interrupción mientras las configuramos.
  cli();
  //Fijamos el registro TCCR1A a 0 para que se mantengan los valores por defecto de la forma de onda
  TCCR1A=0;
  //Activamos el primer y tercer bit (el CS10 y el CS12) del registro TCCR1B para que se utilice como valor de prescaler 1024
  //También activamos en el mismo registro el cuarto bit (WGM12) para que funcione la comparación con el CTC definido
  //La sintaxis habitual es TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
  TCCR1B = B1101;
  //En el registro TIMSK1 activamos el segundo bit (OCIE1A) para que salte la interrupción al llegar al CTC
  //definido en el registro OCR1A
  //Habitualmente se utiliza la siguiente nomenclatura: TIMSK1=(1<<OCIE1A);
  TIMSK1=B10;
  //Definimos el valor del CTC para el timer 1
  OCR1A=15624;
  //Volvemos a activar las interrupciones
  sei();
}

void loop()
{
  //No tenemos nada que ejecutar en el bucle principal
}

ISR(TIMER1_COMPA_vect)
{
  //Función a la que se llama cuando se produce una interrupción
  //Con el parámetro TIMER1_COMPA_vect indicamos que se llama cuando se produce alcanza el CTC definido del timer 1
  
  static volatile byte myCTC = 0; //Contador de segundos (interrupciones) transcurridas

  //Aumentamos nuestro contador y comprobamos si hemos llegado al máximo definido...
  if(++myCTC==myCTCmax)
  {
    //...hemos alcanzado el valor máximo de nuestro contador
    //Reseteamos nuestro contador
    myCTC=0;
    //Ejecutamos la función que nos interesa...
    //Cambiamos el estado del led
    digitalWrite(pinLed,!digitalRead(pinLed));
  }
}

