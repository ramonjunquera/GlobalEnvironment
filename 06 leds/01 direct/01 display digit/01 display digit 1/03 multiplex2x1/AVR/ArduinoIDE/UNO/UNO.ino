/*
  Autor: Ramón Junquera
  Tema: Display de segmentos de led de un dígito
  Objetivo: Aprender qué es multiplexación
  Material adicional: breadboard, cables, 2x 1-digit display, 2 resistencias de 220 ohmios, Arduino UNO

  Descripción:
  Utilizaremos dos displays idénticos de un dígito.
  El objetivo del ejercicio es visualizar los números desde el 00 al 99 secuencialmente.
  El primer problema que nos encontramos es que si un display necesita 8 pines de salida para ser
  controlado, dos necesitarán el doble, 16 pines.
  Pero no tenemos 16 pines digitales de salida en una placa Arduino. Sólo tenemos vemos etiquetados hasta
  el 13.
  El "truco" que utilizaremos se llama multiplexación.
  Aprovecharemos un defecto de nuestra vista llamado persistencia, que consiste en que mantenemos el
  recuerdo visual de una imagen durante unos momentos después de que haya desaparecido.
  Podemos encender y apagar un led muy rápidamente. Más rápido de lo que nuestro ojo es capaz de distinguir.
  Si tenemos dos leds y los hacemos encendemos y apagamos alternativamente lo suficientemente rápido,
  nuestro ojo no verá el parpadeo, sino que asumirá que ambos están encendidos.
  Haremos lo mismo con los dos dígitos. Los encenderemos alternatívamente tan rápido que parezca que ambos
  están encendidos.
  Para ello, utilizaremos 8 pines de salida para conectar los 8 segmentos de ambos display. Los mismos.
  Además utilizaremos 2 pines extra. Cada uno de ellos irá conectado a la pata común de cada display, que
  es la que le proporciona alimentación.
  Escribiremos la configuración del primer dígito, lo alimentaremos, se encenderán los segmentos, esperaremos
  un momento y a continuación lo apagaremos y repetiremos el proceso para el segundo dígito.
  En total serán 10 pines.
  Antes de comenzar un ciclo de parpadeo, nos encargaremos de mantener en estado HIGH el cátodo de ambos displays.
  Esto hace que aunque haya leds con alimentación, no se enciendan porque no hay diferencia de potencial entre
  ellos. Esto mantendrá ambos displays apagados.
  Estos son los pasos que seguiremos en cada ciclo:
  1. Escribiremos la configuración de los pines que controlan los segmentos para mostrar la imagen del display 1
  2. Pondremos el pin del cátodo del display 1 a LOW. Esto hará que se enciendan los segmentos que configuramos.
     Pero sólo lo hacen en el display 1, porque el cátodo del display 2 sigue en HIGH, que lo mantiene apagado.
  3. Los mantendremos encendidos un instante.
  4. Volveremos a poner a HIGH el cátodo del display 1, apagándolo.
  5. Cambiaremos la configuración de los pines que controlan los segmentos para mostrar la imagen del display 2.
  6. Pondremos el estado del cátodo del display 2 a LOW. Esto hace que se enciendan los segmentos configurados
     en el display 2.
  7. Los mantendremos encendidos un instante.
  8. Volveremos a poner a HIGH el cátodo del display 2, apagándolo.

  Comentarios:
  - En este ejercicio no se utiliza el punto decimal. Podríamos no cablearlo.
  - La escritura de un dígito no tiene relación con el display que lo mostrará. Eso de decide después.
  - El tiempo que se tiene que esperar con los leds encendidos para que no se aprecie parpadeo se obtiene a base
    de pruebas. Por eso se ha definido como constante. El valor de 11 milésimas de segundo es el máximo en el
    que no se aprecia el parpadeo.
  - La estructura actual, permite reutilizar código para ampliar el número de displays.
  
  Resultado:
  Vemos cómo se muestran los números del 00 al 99 sin percibir parpadeos
*/

#include <Arduino.h>
#include "RoJoDisplayDigit1.h"

//Declaramos el objeto que gestionará nuestro display
//Como parámetros le pasamos los pines a los que hemos conectado cada segmento
RoJoDisplayDigit1 display = RoJoDisplayDigit1(13,12,11,10,9,8,7,6);
//Array con los pines de los cátodos comunes de cada display
const byte catodePin[] = {2,3};
//Array en la que guardaremos los dígitos del número que queremos mostrar (decenas, unidades)
byte numbers[2];
//Definimos el tiempo en ms que se muestran un dígito en un display
const int delayTime = 11;  

void setup()
{
  //Configuramos los pines de los cátodos comunes como salida y los apagamos
  for(byte i=0;i<2;i++)
  {
    pinMode(catodePin[i],OUTPUT);
    digitalWrite(catodePin[i],HIGH);
  }
}

void loop()
{
  //Mostraremos los números del 0 al 99...
  for(byte n=0;n<100;n++)
  {
    //Calculamos el dígito de las decenas para mostrarlo en el display 1
    numbers[0]=n/10;
    //Calculamos el dígito de las unidades para mostrarlo en el display 2
    numbers[1]=n%10;
    //Repetimos un número determinado de veces el refresco de los dígitos en los displays
    for(byte r=0;r<15;r++)
    {
      //Recorremos las posiciones de los dígitos que componen el número a representar
      for(byte pos=0;pos<2;pos++)
      {
        //Activamos la configuración que muestra el número de la posición actual
        display.set(numbers[pos],false);
        //Activamos ese display
        digitalWrite(catodePin[pos],LOW);
        //Esperamos el tiempo indicado con los leds encendidos
        delay(delayTime);
        //Apagamos el display
        digitalWrite(catodePin[pos],HIGH);
      }
    }
  }
}

