/*
  Autor: Ramón Junquera
  Tema: Características de la placa
  Fecha: 20180803
  Objetivo: Gestión de pines digitales con registros. Lectura
  Material adicional: placa Arduino, led RGB, 3 resistencias de 220 ohmios,
    3 interruptores

  Descripción:
  Continuando con la gestión de pines digitales con registros, haremos un ejercicio en el
  que además de escribir, leemos el estado de algunos pines.

  El circuito consta de un led RGB y tres pulsadores que encenderán cada uno de los colores.

  Asignaremos a los leds tres bits seguidos de un mismo registro.
  Y los mismos bits a los interruptores en otro registro.
  Así será más fácil copiar el estado de los interruptores a los leds.

  Recordemos la distribución de pines en registros:
  
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

  Los interruptores los conectaremos a los pines 2, 3 y 4. Que corresponden a los bits 2, 3 y 4 del registro D.
  Además dejamos libres los pines 0 y 1 del puerto serie.
  Para los leds utilizaremos los pines 10, 11 y 12. Que corresponden a los bits 2, 3 y 4 del registro B.

  En la función setup configuramos los pines de los leds como salida y los apagamos.
  También configuramos los pines de los interruptores como entrada y activamos las resistencias PULLUP.
  
  Puesto que tenemos activas las resistencias internas de PULLUP, si no se pulsa el interruptor, el estado
  leido será HIGH.
  El led RGB utilizado es de ánodo común. Eso quiere decir que para mantener apagado un led debemos aplicar
  el estado HIGH.
  Por lo tanto, el estado del interruptor corresponde con el estado del led: si el interruptor está sin pulsar
  se lee el estado HIGH, que es el mismo estado que debemos aplicar a un led para que se mantenga apagado.
  No es necesario ingún cambio de estado de la lectura de interruptores a la escritura de leds.

  En la función loop leemos el registro de estado de los interruptores y aislamos los bits que nos interesan.
  Por otra parte leemos el estado de los leds y ponemos en estado LOW los pines que nos interesan.
  Sumamos ambos resultados y los volvemos a guardar en el registro de estado de los leds.

  Resultado:
  Los leds se encienden al pulsar los interruptores.
 */

#include <Arduino.h>

void setup()
{
  //Configuramos los pines 10, 11 y 12 como salida (1)
  //Corresponden a los bits 2, 3 y 4 del registro B
  DDRB |= 0b00011100;
  //Ponemos los pines 10, 11 y 12 en estado HIGH (apagamos los leds)
  //Corresponden a los bits 2, 3 y 4 del registro B
  PORTB |= 0b00011100;
  //Configuramos los pines 2, 3 y 4 como entrada (0)
  //Corresponden a los bits 2, 3 y 4 del registro D
  DDRD &= 0b11100011;
  //Activamos las resistencias de PULLUP (1) para los pines 2, 3 y 4
  //Corresponden a los bits 2, 3 y 4 del registro D
  PORTD |= 0b00011100;
}

void loop()
{
  //Copiamos el estado de los pines 2, 3 y 4 del registro PIND (interruptores)
  //en el registro PORTB (leds)
  PORTB = (PORTB & 0b11100011) | (PIND & 0b00011100);
}
