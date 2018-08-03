/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja
  Objetivo: Funcionamiento de una fotorresistencia infrarroja
  Material: breadboard, fotorresistencia infrarroja, led infrarrojo, resistencia de 10Kohmios,
    resistencia de 220 ohmios, pulsador, cables, Arduino UNO y Arduino Nano

  Descripción:
  Este es el primer ejercicio en el que trabajaremos con una fotorresistencia infrarroja.
  Son iguales a las normales, excepto que trabajan con infrarrojo. Al menos esa es la teoría.

  Para hacer las pruebas necesitaremos un emisor y un receptor.
  Para que no haya interferencias entre la emisión y la recepción, crearemos dos circuitos distintos.
  Cada uno con un Arduino y excepto la breadboard, no compartirán nada. Ni siquiera tierra.

  Circuito de recepción.
  Utilizaremos un Arduino Nano.
  Conectaremos la fotoresistencia infrarroja a la primera entrada analógica (A0) y leeremos contínuamente
  sus valores para poder representarlos gráficamente gracias al serial plotter.
  Indicaremos las diferencias entre una fotoresistencia normal y una infrarroja.
    Fotoresistencia normal:
      - No tiene polaridad
    Fotoresistencia infrarroja:
      - Tiene polaridad. Además está intercambiada. La pata larga debe conectarse a tierra y la corta a 5V.
      - Cuanta más luz menos resistencia ofrece.
      - Tiene el aspecto de un led negro. El color oscuro es para que se filtre sólo la luz infrarroja.
  Para asegurarnos de que tenemos una señal baja cuando ofreca altas resistencias, utiizamos una resistencia
  de 10 Kohmios como PULL-DOWN.
      
  Circuito de emisión:
  Utilizaremos un Arduino UNO.
  Es un circuito simple con un led infrarrojo y un interruptor que lo enciende.
  Aunque los leds infrarrojos suelen aguantar bastante bien los 5V por cortos periodos de tiempo sin quemarse,
  tomaremos precauciones y le pondremos una resistencia de 220 ohmios. También entendemos que la intensidad
  de la luz no será la misma.

  En la práctica:
  Sin encender el led IR comprobamos que los valores obtenidos del receptor fluctuan entre el rango de valores
  [15,40]. Esto es normal. Si soltamos el cable del pin A0 y lo dejamos sin conectar, comprobamos que la señal
  también fluctua con una amplitud mucho mayor. Deducimos que las fluctuaciones que tenemos cuando conectamos
  el receptor IR no proceden de él, sino que son normales y su amplitud es más pequeña porque tiene una
  resistencia de 10 Kohmios que la reduce.

  Cuando comenzamos las pruebas de pulsar el interruptor comprobamos que el sensor varía muy ligeramente.
  Practicamente nada. No estamos haciendo nada mal.
  Si probamos a poner una superficie plana sobre los dos leds (emisor y receptor) vemos que entonces sí se puede
  apreciar claramente que el receptor recibe algo. Así que comenzamos a sospechar que el receptor no es
  onmidireccional. Sólo recibe luz por su parte superior. No por los laterales. Así que debería estar orientado
  hacia el led del que procede la luz.

  También podemos ver que debido a la resistencia de 220 ohmnios del emisor, tampoco se ve muy potente.

  Qué ocurre si acercamos una fuente de luz normal al receptor?. Una linterna o el led del móvil.
  Que cuando la luz normal incide directamente se alcanzan unos niveles de resistencia que rozan los máximos.
  Eso quiere decir que el filtro infrarrojo que tiene no sirve de mucho, porque también se activa con luz normal.
  Si se nos ocurre la idea de sustituir el led IR por un led normal (de cualquier color), obtenemos unos resultados
  de aproximadamente la mitad de los conseguidos con el led IR.
  Así que seguimos confirmando que el filtro no es demasiado efectivo.

  Conclusión.
  Para obtener los mejores resultados deberíamos doblar las patas tanto del emisor como del receptor en un ángulo de
  90 grados para que queden encarados uno con el otro.
  Así obtenemos una buena calidad de valores.
*/

#include <Arduino.h>

void setup()
{
  //Activamos el puerto serie
  Serial.begin(115200);
}

void loop()
{
  //Enviamos por el puerto serie el valor analógico leido
  Serial.println(analogRead(A0));
  //Esperamos un momento
  delay(100);
}
