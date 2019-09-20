/*
  Autor: Ramón Junquera
  Tema: Modo de bajo consumo
  Objetivo: Uso del modo de bajo consumo con interruptciones hardware
  Material: breadboard, cables, pulsador, placa Arduino UNO

  Descripción:
  Habitualmente utilizamos como fuente de energía para nuestros proyectos la alimentación USB y no nos preocupamos
  demasiado por su consumo. Pero si el funcionamiento depende de una batería externa debemos pensar cómo podemos
  alargar su duración.
  Los procesadores AtmelMega328p que vienen con Arduino UNO, MEGA o Nano incluyen varios modos de bajo consumo.
  La siguiente tabla muestra los distintos modos y el consumo medio en Arduino UNO:

      Modo                  UNO    Nano
      ------------------- ------- -------
    - Normal              45.00mA 15.00mA
    - SLEEP_MODE_IDLE     15.00mA
    - SLEEP_MODE_ADC       6.50mA
    - SLEEP_MODE_PWR_SAVE  1.62mA
    - SLEEP_MODE_STANDBY   0.84mA
    - SLEEP_MODE_PWR_DOWN  0.36mA
  Describimos algunos con más profundidad:
  - El normal es el que conocemos. Sin ahorro de energía.
  - SLEEP_MODE_IDLE permite que funcione el timer 1 de (16 bits) y la comuniucaciones por puerto serie. Para mejorar
    el ahorro de energía se podrían utilizar funciones adicionales para desactivar otros circuitos, como:
    - power_adc_disable()
    - power_spi_disable()
    - power_timer0_disable()
    - power_timer1_disable()
    - power_timer2_disable()
    - power_twi_disable()
  - SLEEP_MODE_PWR_DOWN sólo tiene en cuenta las interrupciones de harware 0 y 1, además del WatchDog.

  En el siguiente ejemplo se utiliza el modo de máximo ahorro.
  Haremos despertar el dispositivo con una interrupción que tendrá en cuenta un cambio de estado en un pin
  digital. El cambio de estado lo provocará la pulsación del interruptor.

  Los pasos que seguirá la placa serán: "dormir", si se detecta una interrupción, despertar, hacer parpadear el led
  integrado en placa y volver a dormir.

  Esta funcionalidad nos la proporciona una librería que NO es del entorno de desarrollo de Arduino, sino una 
  desarrollada por el propio fabricante (ATMEL). La librería es sleep.h. Habitualmente se encuentra junto con el
  resto de librerías de ATMEL en la carpeta avr.

  Utilizamos las resistencias internas de PULLUP para simplificar el circuito y no tener que añadir una resistencia
  en el pulsador para que mantenga un estado definido mienstras no se pulse.

  Sólo utilizaremos la interrupción 0, correspondiente al estado del pin 2.
  Recordemos que la interrupción 1 corresponde al estado del pin 3. No utilizado en este ejercicio.

  Para entrar en el modo de bajo consumo necesitamos varias instrucciones encadenadas...
    1. set_sleep_mode(modo) Se indica qué modo de bajo consumo utilizaremos. Sólo se define una vez
    2. sleep_enable() Inicializa el modo de bajo consumo. Obligatorio cada vez que queramos entrar.
    3. sleep_mode() Entra en modo de bajo consumo
    4. sleep_disable() Sale del modo de bajo consumo
  
  Resultado:
  La placa se mantendrá en modo de bajo consumo hasta que se pulse el interruptor, que despertará haciendo parpadear
  el led integrado en placa, para volver a bajo consumo.
*/

#include <Arduino.h>

//Librería para usar el modo de bajo consumo
#include <avr/sleep.h>

//Declaración de constantes
const byte pinLed = 13; //Pin del led integrado en placa
const byte pinSwitch = 2; //Pin del interruptor
const byte pinInt=0; //Interrupción asociada al pin del interruptor

void IntSwitch()
{
  //Función a la que se llama cuando cambia el estado del interruptor

  //Despertamos!. Salimos del modo de bajo consumo. Se volverá a repetir la función loop()
  sleep_disable();
}

void setup()
{
  //Configuramos el pin del interruptor como entrada con las resistencias de pullup activas
  pinMode(pinSwitch,INPUT_PULLUP);
  //El pin del led será de salida para poder escribir en él
  pinMode(pinLed,OUTPUT);
  //Definimos el modo de bajo consumo utilizaremos (SLEEP_MODE_PWR_DOWN)
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  //Definimos la interrupción 0 correspondiente al pin 2 cuando pasa de HIGH a LOW (pulsación)
  //Se llamará a la funcíón IntSwitch()
  attachInterrupt(pinInt,IntSwitch,FALLING);
}
 
void loop()
{
  //Inicializamos el modo de bajo consumo
  sleep_enable();
  //Entramos en modo de bajo consumo
  sleep_mode();
  
  //A partir de este punto ya no se ejecuta el programa puesto que estamos "durmiendo"
  //En el momento que despertemos, continuaremos en este punto
  
  //Hacemos parpadear 3 veces el led
  for(byte i=0;i<3;i++)
  {
    digitalWrite(pinLed,HIGH);
    delay(400);
    digitalWrite(pinLed,LOW);
    delay(400);
  }

  //Hemos terminado
  //En la siguiente ejecución de la función loop() volveremos a entrar en modo de bajo consumo
}

