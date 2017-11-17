/*
  Autor: Ramón Junquera
  Tema: Gestión de interrupciones
  Objetivo: Aprender a utilizar la gestión de interrupciones de tiempo de Arduino
  Material: placa Arduino

  Descripción:
  Continuamos con la programación de los timers.
  En el ejercicio anterior conseguíamos programar el timer 1 con un prescaler de 1024 y hacíamos saltar la interrupción
  cada 4,2 segundos.
  Si sólo podemos seleccionar el prescaler no tenemos demasiada libertad para hacer saltar la interrupción cuando queramos.

  Nuestro objetivo en este ejercicio será hacer saltar la interrupción cada segundo (aproximadadmente).

  A partir de ahora, para simplificar, llamaremos CTC al valor en el cual el contador hace saltar la interrupción.
  
  Hasta ahora el CTC venía definido por el la resolución del timer.
  Ahora lo elegiremos nosotros, aunque seguiremos utilizando el prescaler de 1024.
  El CTC nunca será superior al máximo valor que soporta el timer.

  El problema viene ahora... cómo calculamos el CTC para que la interrupción se produzca transcurrido el tiempo deseado?

  Las fórmulas son:
    resolución timer = prescaler / frecuencia
    CTC = (tiempo deseado / resolución timer) -1 = (tiempo deseado * frecuencia / prescaler) -1
    Nota: se le quita un ciclo al CTC porque la interrupción salta en el inicio del último ciclo. No al final.

  Resumiendo. La fórmula mágica es:
    CTC = (tiempo deseado * frecuencia / prescaler) -1
    
  En nuestro caso (sustituimos valores):
    CTC = (1s * 16.000.000Hz / 1024ciclos) -1 = 15624ciclos

  Ahora sólo nos falta saber dónde debemos notificar este CTC para que se tenga en cuenta.
  Esto se guarda en el registro OCR1A.
  Este registro del timer 1 es de 16 bits, así que está formado por 2 bytes: OCR1AH y OCR1AL. Pero Arduino nos facilita el trabajo
  pudiendo asignar el valor de 16 bits directamente a la variable OCR1A sin necesidad de separarlo en sus dos bytes.

  Para activar el modo de comparación del contador con el CTC definido necesitamos conocer un nuevo bit del registro TCCR1B.
  El bit 3, también llamado WGM12, sirve para ésto.

  También tendremos que recordar que debemos cambiar algo en el registro TIMSK1.
  Refrescamos el significado de los bits del registro TIMSK1:
  - El bit 0 (TOIE1) se activaba cuando queríamos que el CTC fuese el máximo valor permitido (ejercicios anteriores).
    Pero ahora no es el caso. Nosotros definimos el CTC y lo guardamos el el registro OCR1A.
  - El bit 1 (OCIE1A) se debe activar cuando queremos que se tenga en cuenta el valor del registro OCR1A como CTC.
    Este en nuestro caso ahora.

  Resultado:
  El led integrado en placa parpadea cada segundo.
*/

#include <Arduino.h>

//Como trabajaremos directamente con los registros del procesador, incluiremos las librerías de gestión del fabricante
//del procesador. No hacemos referencia a librerías de Arduino.
#include <avr/io.h>
#include <avr/interrupt.h>

//Declaración de variables globales
const byte pinLed = 13; //Pin al que está conectado el led

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

  //Cambiamos el estado del led
  digitalWrite(pinLed,!digitalRead(pinLed));
}

