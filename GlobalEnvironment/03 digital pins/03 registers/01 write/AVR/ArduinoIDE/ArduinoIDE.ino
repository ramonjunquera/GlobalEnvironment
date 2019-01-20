/*
  Autor: Ramón Junquera
  Tema: Características de la placa
  Fecha: 20180803
  Objetivo: Gestión de pines digitales con registros. Escritura.
  Material adicional: placa Arduino, 6 leds, resistencia de 220 ohmios

  Descripción:
  El IDE de Arduino nos ofrece algunos comandos para gestionar los pines digitales:
  - pinMode
  - digitalWrite
  - digitalRead
  Cada comando afecta sólo a un pin.

  Hay otra forma de gestionar los pines digitales... a través de registros.

  Las funciones para gestionar un registro son las mismas que nos ofrece Arduino:
  - Configurar un pin como entrada o salida
  - Escribir el estado de un pin
  - Leer el estado de un pin
  Para ello se utilizan tres tipos de registros:
  - DDRx: configurar un un pin como entrada o salida
  - PORTx: escribir el estado de un pin
  - PINx: leer el estado de un pin

  Un registro es un byte, que además tiene asignada una variable para facilitar su acceso.
  Cada uno de los bits de un registro corresponde a un pin físico.
  Puesto que tenemos más de 8 pines, necesitamos más de un registro. Los diferenciaremos con una letra.
  Usaremos los registros B, C y D para poder gestionar todos los pines físicos, al menos en Arduino
  UNO o Nano.

  El registro D gestiona los pines desde el 0 hasta el 7.
  El registro B los pines a partir del 8 al 13
  Y el registro C los pines analógicos.

  En la siguiente tabla se muestran los pines que controla cada uno de los bits de cada registro

      Reg.D Reg.B Reg.C
  bit  pin   pin   pin
  --- ----- ----- -----
   0    0     8   A0=14
   1    1     9   A1=15
   2    2    10   A2=16
   3    3    11   A3=17
   4    4    12   A4=18
   5    5    13   A5=19
   6    6    --   A6=20
   7    7    --   A7=21

  Arduino UNO no tiene los pines A6 y A7, pero Nano sí.

  Recordemos que los pines analógicos pueden ser utilizados como digitales.
  Su numeración continúa a los del pines del registro B
  
  Si nos fijamos en una placa Arduino UNO veremos que incluso los pines están distribuidos en estos grupos.

  Explicaremos la funcionalidad de los tipos de registros...

  Lo primero que hacemos en Arduino para trabajar con un pin digital es fijar su modo con la instrucción pinMode.
  De esto se encarga el registro DDRx.
  Un bit a 0 = INPUT, y a 1 = OUTPUT.

  Aparte de los modos INPUT y OUTPUT, tenemos el modo INPUT_PULLUP, que utiliza unas resistencias internas para mantener 
  en estado HIGH el pin de entrada.
  Esto de puede hacer de dos maneras distintas. Con un comando:
    Ej. pinMode(pin,INPUT_PULLUP);
  Con dos comandos:
    Ej. pinMode(pin,INPUT); digitalWrite(pin,HIGH);
  En este último caso, fijamos el pin como entrada y aunque sea entrada indicamos que su valor por defecto será HIGH.

  Con registros funciona igual.
  El registro DDRx sólo nos permite fijar si el pin es INPUT o OUTPUT.
  Con el registro PORT fijamos/escribimos el valor del pin.
  Si es un pin de entrada, indicamos el estado por defecto cuando no está conectado:
    1 - HIGH gracias a las resistencias de PULLUP
    0 - indeterminado
  Si el pin es de salida, simplemente escribimos su valor.

  Nos falta un último caso. Cuando el pin es de entrada y queremos leer su valor. Para ello utilizamos el registro PINx.
  Este es el único registro que es sólo de lectura. Los anteriores (DDRx y PORTx) son de lectura/escritura.

  Los bits 6 y 7 del registro B no corresponden a ningún pin. Realmente se utilizan para controlar la velocidad
  del oscilador interno. No deberían ser afectados por nuestras escrituras.
  
  Ejemplos:

  Pin:4. Modo: salida. Estado: HIGH
  Usaremos el pin 4 del registro D.
  DDRD |= (1<<4); //pinMode(4,OUTPUT);
  PORTD |= (1<<4); //digitalWrite(4,HIGH);

  Pin:13. Modo salida. Estado: LOW
  Usamos el pin 5 del resgistro B.
  DDRB |= (1<<5); //pinMode(13,OUTPUT);
  PORTB &= ~(1<<5); //digitalWrite(13,LOW);

  Pin:A4. Modo entrada sin pullup.
  Usamos el pin 18 que corresponde con el pin 4 del registro C
  DDRC &= ~(1<<4); //pinMode(18,INPUT);
  PORTC &= ~(1<<4); //No usaremos resistencias de PULLPUP digitalWrite(18,LOW);
  value = PINC & (1<<5); //Obtenemos el valor de del pin 18

  Pin:7: Modo entrada con pullup.
  Usamos el pin 7 del registro D
  DDRD &= ~(1<<7); //pinMode(7,INPUT);
  PORTD |= (1<<7); //digitalWrite(7,HIGH); activamos las resistencias de PULLUP
  value = PIND & (1<<7); //Obtenemos el valor del pin 7

  En el ejercicio actual se trabajará con los registros D que afectan a los 8 primeros pines.
  Puesto que los dos primeros no deberíamos utilizarlos si no queremos tener problemas con
  el puerto serie hardware, utilizaremos los pines del 2 al 7. En total 6.
  Los configuraremos todos de salida.
  Comenzaremos con todos apagados.
  Iremos encendiendo cada uno de ellos secuencialmente y repetiremos.
  Puesto que nunca encenderemos más de un led al mismo tiempo, podremos utilizar una única
  resistencia para todos ellos.

  Nota:
  Si utilizamos una placa MakerUNO no son necesarios elementos externos porque ya tiene
  integrados los leds para los pines.

  Resultado:
  Los leds se encienden secuencialmente
 */

#include <Arduino.h>

void setup()
{
  //Configuramos todos los pines del registro D como salida (1), excepto los dos primeros que mantendrán su configuración
  DDRD |= 0b11111100;
  //Apagamos todos los leds
  PORTD &= 0b00000011;
}

void loop()
{
  //Recorremos todos los leds que utilizamos en el ejemplo (del 2 al 7)...
  for(byte led=2;led<8;led++)
  {
    //Encendemos el led que corresponde
    PORTD |= (1<<led);
    //Esperamos un momento
    delay(300);
    //Apagamos todos los leds
    PORTD &= B00000011;
  }
}
