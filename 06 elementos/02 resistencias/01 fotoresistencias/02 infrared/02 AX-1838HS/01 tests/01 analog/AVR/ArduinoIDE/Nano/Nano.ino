/*
  Autor: Ramón Junquera
  Tema: Fotorresistencia infrarroja AX-1838HS
  Objetivo: Funcionamiento de una fotorresistencia infrarroja AX-1838HS
  Material: breadboard, fotorresistencia infrarroja AX-1838HS, led infrarrojo, pulsador, cables, Arduino Nano

  Descripción:
  En la siguiente serie de proyectos analizaremos el comportamiento del receptor infrarrojo AX-1838HS.

  En este primer ejercicio comprobaremos la fiabilidad de los filtros infrarrojos.
  Conectaremos la pata de señal del sensor a la primera entrada analógica (A0) y mostraremos su lectura contínuamente.

  Para comprobar cómo se comporta ante la luz IR, utilizaremos un led IR.
  Como ya aprendimos con las fotoresistencias IR normales, doblaremos sus patas, porque sabemos que es direccional.
  El led emisor apuntará directamente al receptor.
  Para ahorrarnos la resistencia del led emisor, lo conectaremos a 3.3V.
  
  El led emisor lo podremos activar a voluntad gracias un pulsador.
  No gestionaremos el pulsador con Arduino. No es el objetivo de esta práctica. Utilizamos un simple circuito
  eléctrico.

  Una vez montado el circuito, podemos asegurarnos que el emisor infrarrojo funciona correctamente al pulsar el
  interruptor mediante la cámara del móvil, puesto que nuestros ojos no pueden ver ese tipo de luz.

  Podemos utilizar el serial plotter para que muestre en una gráfica los valores recibidos.
  Es mucho más visual.

  El programa es muy simple. Sólo muestra contínuamente el valor leido por la primera entrada analógica.

  En la práctica...
  Recordemos que el rango de valores que recibe un pin analógico va desde 0 a 1023.
  Lo primero que vemos es que sin encender ninguno de nuestros leds, el valor leido es el máximo (1023).
  Incluso con una luz ambiental fuerte o solar, vemos que no afecta al resultado.
  Podriamos pensar que los filtros IR están funcionando correctanmente.

  Si el sensor tiene led de actividad y encendemos el led emisor IR, comprobamos que se parpadea por unos instantes.
  Pero lo que no siempre vemos es que el valor leído cambie.
  Cuando alguna vez conseguimos capturar un valor distinto al máximo, comprobamos que es muy bajo (del orden de 50).

  Cuando pulsamos (encendemos) el led IR vemos que parpadea el led de actividad del receptor por un segundo (aprox.).
  Pero en cambio no parece parpadear cuando lo soltamos.

  La conclusión es que el sensor no entrega una señal analógica acorde con el luz IR recibida, sino que parece entregar
  una señal digital que además no se mantiene en el tiempo.
  La señal sólo aparece cuando hay cambios de intensidad.
  
  Otra diferencia que encontramos frente a una fotoresistencia estándard es el tiempo de respuesta.
  Según las especificaciones del fabricante tiene un tiempo de respuesta de 80ms que permite que se pueda enviar pequeñas
  cantidades de información modulando la frecuencia, tal y como lo hacen la mayoría de mandos a distancia del mundo.
  
  Resultado:
  Comprobamos que los filtros del AX-1838HS hacen correctamente su trabajo y que parece que la señal que genera no es analógica.
  Además la señal sólo aparece cuando hay cambios de intensidad.
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
  delay(50);
}
