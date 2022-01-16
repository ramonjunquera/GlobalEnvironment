/*
  Autor: Ramón Junquera
  Fecha: 20191030
  Tema: Modo de bajo consumo del ATtiny85
  Objetivo: Despertar por interrupciones hardware
  Material adicional: breadboard, placa ATtiny85, interruptor de pulsador

  Descripción:
  La placa ATtiny85 tiene un consumo muy reducido.
  En el proyecto presentado consume unos 12mA en el arranque y unos 13mA si se dedica a hacer
  parpadear el led integrado.

  Suponiendo que una pila de botón del tipo CR2032 es capaz de suministrar 200mAh, tendríamos
  que puede alimentar un ATtiny85 durante 200mAh/12mA ~ 16h.

  En el ejercicio veremos cómo se puede entrar en modo de bajo consumo hasta reducirlo a
  4mA. Con el ejemplo de la pila CR2032 nos daría una autonomía de 200mAh/4mA = 50h.

  El ATtiny85 tiene tiene 3 modos de operación, tal y como se explica en el datasheet
  del fabricante, en el punto 7.1:
  - Idle : Modo normal. Funcionan todos los servicios
  - ADC noise Reduction: Se elimina algún servicio
  - Power-down: Se eliminan la mayoría de servicios
  El modo Power-down es el que menos energía consume.
  Una vez que se entra en este modo, sólo se puede salir por una interrupción de hardware (cambio
  de estado en un pin), o por una interrupción del watchdog (un timer especial).

  En el ejemplo explicaremos la primera opción.
  Tendremos un interruptor conectado a un pin.
  Configuraremos las interrupciones para que detecte cualquier cambio de estado en ese pin.
  Esta interrupción provocará que se salga al momento del modo de power_down.

  Para notificar que estamos en modo normal, haremos parpadear el led integrado varias veces.

  El primer problema que nos encontramos es que la definición de interrupciones a través de
  la API de Arduino NO funciona!.
  Debemos configurar las interrupciones a través de registros.
  Esta definición la hacemos una sóla vez a lo largo del programa. Se encuentra en la función setup()

  Recordemos que la definición de interrupciones por registros permite utilizar cualquier pin, no
  sólo el 2 como con las instrucciones de Arduino.
  Además, no podremos definir el tipo de interrupción (tipo de flanco). Todas las interrupciones
  serán del tipo CHANGE.

  La función loop hará parpadear el led y a continuación entrará en modo de bajo consumo.
  Hasta que no salga de él, no continuará.

  Aunque entrar en modo de bajo consumo sólo es una instrucción (sleep_mode()), crearemos una función
  porque añadiremos algo más...

  Unos de los servicios que tiene el ATtiny85 es el ADC: Analog Digital Conversion = sistema de conversión
  de analógico a digital.
  Sirve para digitalizar una señal analógica. Imprescindible si se pretende leer el valor de entrada de
  un pin analógico.
  Este servicio siempre está en marcha. Y siempre tiene un consumo aproximado de unos 0,22mA.
  No es mucho, pero si pretendemos reducir al máximo el consumo, deberíamos pensar en ello.

  La función Sleep() es la encargada de poner a ATtiny85 en modo de bajo consumo, pero antes de hacerlo
  desactiva el ADC.
  Instantes después de salir, lo vuelve a activar para dejarlo normal.

  Hay veces que en vez de utilizar sleep_mode() se usa sleep_cpu().
  La diferencia es que sleep_mode() hace lo mismo que:
    sleep_enable(); sleep_cpu(); sleep_disable();
  Como la llamada a las funciones sleep_enable y sleep_disable son obligatorias, nos ahorramos tener que
  escribirlas nosotros.

  La función sleep_cpu() entra en el modo de bajo consumo que se ha definido previamente con la función
  set_sleep_mode().

  La función sleep_enable() se encarga de activar el bit SE del registro MCUCR: MCUCR |= (1<<SE);
  La función sleep_disable() se encarga de desactivar el bit SE del registro MCUCR: MCUCR $= ~(1<<SE);

  Resultado:
  ATtiny85 entra en modo de bajo consumo y no sale hasta que no pulsamos el interruptor.
  En ese momento hace parpadear el led integrado y vuelve al modo de bajo consumo.
*/

#include <Arduino.h>
#include <avr/sleep.h> //Gestión del modo de bajo consumo
#include <avr/interrupt.h> //Gestión de interrupciones

//Declaración de constantes globales
const byte pinLed = 1; //Pin del led integrado en placa
const byte pinSwitch = 0; //Pin del interruptor

void setup() {
  //El pin del interruptor será de lectura y activaremos las resistencias internas de pullup
  pinMode(pinSwitch,INPUT_PULLUP);
  //El pin del led será de escritura
  pinMode(pinLed,OUTPUT);

  //Definimos el modo de bajo consumo. Utilizaremos PWR_DOWN
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  //Desactivamos las interrupciones
  cli();
  //Activamos las interrupciones hardware por cambio de estado de un pin
  //Esto se hace activando el bit PCIE del registro GIMSK
  //También se puede escribir como GIMSK |= _BV(PCIE);
  GIMSK |= (1<<PCIE);
  //Activamos las interrupciones hardware para el pin del interruptor
  PCMSK |= (1<<pinSwitch);
  //Activamos las interrupciones
  sei();
}

void sleep() {
  //Función para entrar en modo de bajo consumo

  //Desactivamos el ADC (sistema de conversión analógica a digital) para consumir menos
  //Esto se hace poniendo a 0 el ADEN (7) del registro ADCSRA
  //También se podría escribir como: ADCSRA &= ~_BV(ADEN);
  ADCSRA &= ~(1<<ADEN);
  //Entramos en modo de bajo consumo
  sleep_mode();
  //A partir de este momento estamos en modo de bajo consumo
  //No continuaremos con la ejecución del programa hasta salir de él
  //En este caso sólo saldremos cuando se detecte una interrupción por hardware

  //Activamos el ADC
  ADCSRA |= (1<<ADEN);
}

ISR(PCINT0_vect) {
  //Función a la que se llama ante cualquier interrupción
  //El parámetro PCINT0_vect indica que se trata de interrupciones por hardware
  //Si llegamos aquí es porque hemos salido del modo de bajo consumo
  //No necesitamos tomar acciones especiales
}

void loop() {
  //Hacemos parpadear 3 veces el led
  for(byte i=0;i<3;i++) {
    digitalWrite(pinLed,HIGH);
    delay(200);
    digitalWrite(pinLed,LOW);
    delay(200);
  }
  //Entramos en modo de bajo consumo
  sleep();

  //No se llegará hasta este punto hasta que no se salga del modo de bajo consumo
}
