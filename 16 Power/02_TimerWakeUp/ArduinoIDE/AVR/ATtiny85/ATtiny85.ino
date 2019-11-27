/*
  Autor: Ramón Junquera
  Fecha: 20191030
  Tema: Modo de bajo consumo del ATtiny85
  Objetivo: Despertar por interrupciones hardware
  Material adicional: breadboard, placa ATtiny85, interruptor de pulsador

  Descripción:
  Watchdog es un timer especial.
  El procesador sólo tiene un watchdog.
  Cuando el contador de watchdog llega al valor máximo (overflow) genera una interrupción que
  es capaz de salir del modo de bajo consumo.
  Watchdog también tiene prescalers, igual que cualquier otro timer.
  El prescaler del watchdog se guarda en el registro WDTCSR.
  A continuación se muestra la tabla de los distintos valores de prescalers, el número de 
  ciclos de reloj a los que corresponde y el tiempo aproximado de dura:
  
  prescaler ciclos  tiempo
  --------- ------- -------
      0        2048    16ms
      1        4096    32ms
      2        8192    64ms
      3       16384   125ms
      4       32768  0.25s
      5       65536   0.5s
      6      131072     1s
      7      262144     2s
      8      524288     4s
      9     1048576     8s
  
  El ejercicio es similar al anterior. ATtiny85 estará en modo de bajo consumo durante la 
  mayor parte del tiempo.
  Watchdog se encarga de hacer salir de este modo periódicamente.

  El modo de bajo consumo utilizado es el mismo que el usado en el ejemplo de la activación por hardware,
  power-down (SLEEP_MODE_PWR_DOWN), porque es el que menos consume.
  El consumo sigue siendo también el mismo. Unos 4mA.
  También nos encargamos de desactivar el sistema ADC.

  Los pasos de la función setup() son:
  - Configuración del modo del pin del led
  - Configuración de watchdog con el prescaler adecuado
  - Definición del modo de bajo consumo

  Una vez en el loop():
  - Hacemos parpadear el led
  - Entramos en modo de bajo consumo

  Se han creado funciones independientes para las acciones más importantes:
  - Configuración de watchdog
  - Entrar en modo de bajo consumo

  La función a la que se llama cuando se produce la interrupción de watchdog está vacía
  porque no hay acciones especiales a ejecutar. Nos conformamos con salir del modo
  de bajo consumo.
  
  Resultado:
  ATtiny85 está siempre en modo de bajo consumo. Periódicamente watchdog hace despertar 
  al sistema, que hace parpadear el led integrado, y vuelve a dormir.
*/

#include <Arduino.h>
#include <avr/sleep.h> //Gestión del modo de bajo consumo
#include <avr/wdt.h> //Gestión del watchdog

//Declaración de constantes globales
const byte pinLed = 1; //Pin del led integrado en placa

//Activa el watchdog con el prescaler indicado para que cuando su contador
//llegue al final no haga un reset, sino que genere una interrupción que
//permita salir del modo de bajo consumo
void setupWatchdog(int watchdogPrescaler) {
  //Los posibles valores de watchdogPrescaler son:
  // 0 -> 16ms
  // 1 -> 32ms
  // 2 -> 64ms
  // 3 -> 128ms
  // 4 -> 250ms
  // 5 -> 500ms
  // 6 -> 1s
  // 7 -> 2s
  // 8 -> 4s
  // 9 -> 8s

  //Crearemos una variable en la que guardaremos el valor de la configuración final que
  //queremos aplicar al registro WDTCR
  byte WDTCRfinalConfig;
  //Principalmente calcularemos el valor del prescaler e indicaremos que cuando el
  //contador del watchdog llegue a su máximo valor (overflow), se genere una interrupción
  //que nos permita salir del modo de bajo consumo

  //Nos entregan un valor para aplicar al prescaler
  //Esto queda definido por 4 bits del registro WDTCR: WDP0, WDP1, WDP2 y WDP3
  //El problema es que los bits no son contiguos!

  //Recordemos la estura del registro WDTCR - WatchDog Timer Control Register
  //  bit 7 - WDIF: WatchDog Timeout Interrupt Flag
  //  bit 6 - WDI: WatchDog Timeout Interrupt Enable
  //  bit 5 - WDP3 : WatchDog Prescaler bit 3
  //  bit 4 - WDCE : WatchDog Change Enable
  //  bit 3 - WDE : WatchDog Enable
  //  bit 2 - WDP2 : WatchDog Prescaler bit 2
  //  bit 1 - WDP1 : WatchDog Prescaler bit 1
  //  bit 0 - WDP0 : WatchDog Prescaler bit 0

  //Como los 3 bits más bajos (0, 1 y 2) están juntos, nos encargaremos primero de ellos
  //Aislamos los 3 primeros bits
  WDTCRfinalConfig = watchdogPrescaler & 7;
  //Nos falta el bit más alto del prescaler
  //Si el código del prescaler tiene bit 3...activaremos el bit 5 del valor final
  //Porque el bit 3 del prescaler se guarda en el bit 5 de WDTCR
  if (watchdogPrescaler > 7) WDTCRfinalConfig |= (1<<5);
  //Indicamos que cuando el contador del watchdog llege al máximo valor debe generar una
  //interrupción, activando el bit WDIE del registro WDTCR
  WDTCRfinalConfig |= (1<<WDIE);
  //Hemos terminado de calcular la configuración final del registro WDTCR

  //Borramos el flag de reset de watchdog
  //El flag corresponde al bit WDRF (WatchDog Reset Flag). Que es el bit 3. Dentro
  //del registro MCUSR.
  //Al desactivarlo conseguimos que cuando el contador del watchdog llegue al máximo, NO
  //resetee el ATtiny85
  MCUSR &= ~(1<<WDRF);
  
  //Activamos el watchdog con el bit WDE del registro WDTCR
  //Además activamos el bit WDCE para que se guarde la modificación
  //El bit WDCE volverá a 0 en 4 ciclos de reloj
  WDTCR |= (1<<WDE) | (1<<WDCE);
  //Antes de que pasen esos 4 ciclos se debe aplicar la configuración para que sea válida
  //Por esa razón hacemos su cálculo previamente
  //Si hacemos el cálculo aquí, tarderemos más de 4 ciclos y para cuando la apliquemos
  //estaremos fuera de tiempo y no se tendrá en cuenta

  //Aplicamos el valor final calculado para el registro WDTR
  WDTCR = WDTCRfinalConfig;
}

//Función para entrar en modo de bajo consumo
void sleep() {
  //Desactivamos el ADC (sistema de conversión analógica a digital) para consumir menos
  //Esto se hace poniendo a 0 el ADEN (7) del registro ADCSRA
  //También se podría escribir como: ADCSRA &= ~_BV(ADEN);
  ADCSRA &= ~(1<<ADEN);
  //Entramos en modo de bajo consumo
  sleep_mode();
  //A partir de este momento estamos en modo de bajo consumo
  //No continuaremos con la ejecución del programa hasta salir de él
  //En este caso sólo saldremos cuando se watchdog salte

  //Activamos el ADC
  ADCSRA |= (1<<ADEN);
}

ISR(WDT_vect) {
  //Función a la que se llama ante cualquier interrupción
  //El parámetro WDT_vect indica que se trata de interrupciones generadda por watchdog
  //La interupción se genera cuando el contador de watchdog llega a su valor máximo (overflow)
  //Si llegamos aquí es porque hemos salido del modo de bajo consumo
  //No necesitamos tomar acciones especiales
}

void setup() {
  //El pin del led será de escritura
  pinMode(pinLed,OUTPUT);
  //Definimos el tiempo que debe tardar watchdog en generar una interrupción
  //Con un prescaler de 6 conseguimos un tiempo de un segundo (aproximadamente)
  setupWatchdog(6);
  //Definimos el modo de bajo consumo. Utilizaremos PWR_DOWN, el que menos consume
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop() {
  //Hacemos parpadear el led
  digitalWrite(pinLed,HIGH);
  delay(200);
  digitalWrite(pinLed,LOW);
  //Entramos en modo de bajo consumo
  sleep();

  //No se llegará hasta este punto hasta que no se salga del modo de bajo consumo
}
